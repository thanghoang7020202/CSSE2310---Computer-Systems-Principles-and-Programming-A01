#ifndef support_h
#define support_h

#define MAX_WORD_LENGTH 50
#define MAX_WORDS 5
#define DICTIONARY_FILE "/usr/share/dict/words"
#define INITIAL_BUFFER_SIZE 100

char* read_line(FILE* stream) {

    int bufferSize = INITIAL_BUFFER_SIZE;
        char* buffer = malloc(sizeof(char) * bufferSize);
        int numRead = 0;
        int next;

    if (feof(stream)) {
        return NULL;
    }
    while (1) {
        next = fgetc(stream);
        if (next == EOF && numRead == 0) {
            free(buffer);
            return NULL;
        }
        if (numRead == bufferSize - 1) {
            bufferSize *= 2;
            buffer = realloc(buffer, sizeof(char) * bufferSize);
        }
        if (next == '\n' || next == EOF) {
            buffer[numRead] = '\0';
            break;
        }
        buffer[numRead++] = next;
    }
    return buffer;
}

char** filetokenize(FILE* fp) {
    char** wordLib = (char**)malloc(sizeof(char*)*5);
    int wordLibSize = 5;
    char* line = NULL;
    int i = 0;
    for(line = read_line(fp); line ; line = read_line(fp), i++) {
        if (i == (wordLibSize - 1)) {
            wordLibSize *= 2;
            wordLib = realloc(wordLib, sizeof(char*) * wordLibSize);
        }
        wordLib[i] = line;
        //printf("%s\n", wordLib[i]);
    }
    //free(line);
    return wordLib;
}

void errorExitOne() {
    fprintf(stderr, "Usage: uqwordiply [--start starter-word | --len length]"
      " [--dictionary filename]\n");
    exit(1);
}

#endif /* support_h */