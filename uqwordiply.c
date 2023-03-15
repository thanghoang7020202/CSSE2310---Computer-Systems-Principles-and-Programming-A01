#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
//#include <csse2310a1.h>
//#include "support.h"

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

char* toLower(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}

char* toUpper(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        word[i] = toupper(word[i]);
    }
    return word;
}

char** filetokenize(FILE* fp) {
    char** wordLib = (char**)malloc(sizeof(char*)*5);
    int wordLibSize = 5;
    char* line = NULL;
    int i = 0;
    for(line = read_line(fp); line ; line = read_line(fp), i++) {
        if (i == (wordLibSize - 2)) {
            wordLibSize *= 2;
            wordLib = (char**)realloc(wordLib, sizeof(char*) * wordLibSize);
        }
        line = toUpper(line);
        wordLib[i] = line;
        //printf("%s\n", wordLib[i]);
    }
    free(line);
    wordLib[i +1] = NULL;
    return wordLib;
}

void errorExitOne() {
    fprintf(stderr, "Usage: uqwordiply [--start starter-word | --len length]"
      " [--dictionary filename]\n");
    exit(1);
}

bool isLibWord(char* word, char** wordLib) {
    for (int i = 0; wordLib[i] != NULL; i++) {
        if (strcmp(word, wordLib[i]) == 0) {
            wordLib[i] = "0";
            return true;
        }
    }
    return false;
}

bool isAlpha(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        if (!isalpha(word[i])) {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    char* dictionary = DICTIONARY_FILE;
    char* starterWord = NULL;
    int maxWords = 0;
    for(++argv, --argc; argc > 0 ; argc --, argv++) {
        //printf("argc = %d, argv = %s\n",argc, *argv);
        if(strcmp(*argv, "--start") == 0) {
            if(maxWords != 0 || starterWord != NULL || *(argv+1) == NULL) {
                errorExitOne();
            }
            starterWord = *(++argv);
            --argc;
            
            if (strlen(starterWord) != 3 && strlen(starterWord) != 4 || !isAlpha(starterWord)) {
                fprintf(stderr, "uqwordiply: invalid starter word\n");
                exit(2);
            }
        } else if (strcmp(*argv, "--dictionary") == 0) {
            dictionary = *(++argv);
            if (dictionary == NULL) {
                errorExitOne();
            }
            --argc;
        } else if (strcmp(*argv , "--len") == 0) {
            if (starterWord != NULL || maxWords != 0 || *(argv+1) == NULL) {
                errorExitOne();
            }
            char* cNumber = *(++argv);
            //printf("cNumber is: %s", cNumber);
            --argc;
            if (strcmp(cNumber,"3") == 0) {
                maxWords = 3;
            } else if (strcmp(cNumber,"4") == 0) {
                maxWords = 4;
            } else {
                errorExitOne();
            }
        } else {
            errorExitOne();
        }
    }

    // test basic input first
    // open dictionary file, fix path later 
    char** guessedWord = (char**)malloc(sizeof(char*)* MAX_WORDS);
    for (int i = 0 ; i < MAX_WORDS; i++) {
        guessedWord[i] = (char*)malloc(sizeof(char) * MAX_WORD_LENGTH);
        //why can't I use guessedWord[i] = "0"; here? it lead to free memory error!
        guessedWord[i] = "0";
    }
    FILE* fp = fopen(dictionary, "r");
    dictionary = NULL;
    free(dictionary);

    if(fp == NULL) {
        printf("uqwordply: dictionary file \"%s\" cannot be opened\n", dictionary);
        exit(3);
        return 0;
    }
    
    char** words = filetokenize(fp);

    if (words == NULL) {
        printf("This file can't be tokenized!");
        return 0;
    }
    
    if (starterWord == NULL) {
        starterWord = "top";
        //starterWord = get_wordiply_starter_word(maxWords);
    }
    printf("Wellcome to UQWordiply!\n");
    printf("The starter word is: %s\n", starterWord);
    printf("Enter words containing this word.\n");
    for (int idx = 1; idx < MAX_WORDS +1;) {
        char* guess = (char*)malloc(sizeof(char)*MAX_WORD_LENGTH);
        //recieve input from user
        printf("Enter guest %d: ",idx);
        scanf("%s", guess);
        // checking process
        if (feof(stdin)) {
                exit(4);
        }

        //printf("the input is: %s\n", guess); //----------------------
        
        if (strcmp(guess, starterWord) == 0) {
            printf("Guesses can't be the starter word - try again.\n");
            continue;
        } 
        // invalid char check
        // (guess[j] >= 65 && guess[j] <= 90) || (guess[j] >= 97 && guess[j] <= 122)
        if (!isAlpha(guess)) {
            // These number above are ASCII code for A-Z and a-z
            printf("Guesses must contain only letters - try again.\n");
            continue;
        } 

        // starter word check
        //printf("Starter word check!\n");//----------------------

        if (strstr(guess, starterWord) == NULL) {
            printf("Guesses must contain the starter word - try again.\n");
            continue;
        } 
        
        // existed word check
        //printf("Existed word check!\n");//----------------------

        for(int i = 0; strcmp(guessedWord[i],"0"); i++) {
            if (strcmp(toUpper(guess),toUpper(guessedWord[i])) == 0) {
                printf("You already guessed that word - try again.\n");
                continue;
            }   
        }
        
        // search word in library
        //printf("Lib search!\n");//----------------------
        
        if (isLibWord(guess, words)) {
            guessedWord[idx] = guess;
            idx ++;
            continue;
        }else {
            printf("Guess not found in words - try again.\n");
            continue;
        }
    }
    //printf("1\n");
    fclose(fp);
    printf("2\n");
    for (int i =0; i < MAX_WORDS; i++) {
        printf("Loop no: %d\n",i);
        free(guessedWord[i]);
    }
    printf("3\n");
    free(guessedWord);
    printf("4\n");
    printf("5\n");
    free(words);
    return 0;
}