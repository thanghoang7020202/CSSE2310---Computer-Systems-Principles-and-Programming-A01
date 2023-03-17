#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <csse2310a1.h>
//#include "support.h"

#define MAX_WORD_LENGTH 52
#define MAX_WORDS 5
#define DICTIONARY_FILE "/usr/share/dict/words"
#define INITIAL_BUFFER_SIZE 100

typedef struct {
    char** wordList;
    int wordLibSize;
    int longestLen;
} Lib;

typedef struct {
    char* dictionary;
    char* starterWord;
    int maxWords;
}TerPara;

/**
 * @brief Check whether the word containing all letters
 * 
 * @param word word to check
 * @return true if word contain only letters
 * @return false if word contain any character that not a letter
 */
bool is_alpha(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        if (isalpha(word[i]) == 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief read each line from file stream
 * 
 * @param stream file stream to be read
 * @return char* return buffer of read words
 */
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
            exit(4);
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

/**
 * @brief change all word input to lower case
 * 
 * @param word word input word
 * @return char* lower cased word
 */
char* to_lower(char* word) {
    if (word == NULL) {
        return NULL;
    }
    for (int i = 0; i < strlen(word); i++) {
        word[i] = tolower(word[i]);
    }
    return word;
}

/**
 * @brief change all word input to upper case
 * 
 * @param word input word
 * @return char* upper cased word
 */
char* to_upper(char* word) {
    if(word == NULL) return NULL;
    for (int i = 0; i < strlen(word); i++) {
        word[i] = toupper(word[i]);
    }
    return word;
}

/**
 * @brief Add line to a list (array)
 * 
 * @param wordLib array to add in
 * @param line word to add in array
 */
void add(Lib* wordLib, char* line) {
    int len = strlen(line);
    char* copyline = strdup(line);
    wordLib->wordList = realloc(wordLib->wordList,
            sizeof(char*) * wordLib->wordLibSize + 1);
    wordLib->wordLibSize++;
    wordLib->wordList[wordLib->wordLibSize - 1] = copyline;
    // compare to update longest word!
    if (wordLib->longestLen < len) {
        wordLib->longestLen = len;
    }
}

/**
 * @brief separate each word in library 
 * and put them in a list (array)
 * 
 * @param fp file point to read
 * @param starterWord inital word
 * @return Lib dictionary struct contain separated word
 */
Lib file_tokenize(FILE* fp, char* starterWord) {
    Lib wordLib;
    wordLib.wordList = NULL;
    wordLib.wordLibSize = 0;
    wordLib.longestLen = 0;
    char line[MAX_WORD_LENGTH];
    
    for (; fgets(line, MAX_WORD_LENGTH, fp);) {
        int lengthLine = strlen(line);
        // replace newline with NULL terminator
        line[lengthLine - 1] = '\0';
        
        if (!is_alpha(line)) {
            continue;
        }
        add(&wordLib, line);
    }
    return wordLib;
}

/**
 * @brief function to exit 1 and print structure of commandline input
 */
void error_exit_one() {
    fprintf(stderr, "Usage: uqwordiply [--start starter-word | --len length]"
            " [--dictionary filename]\n");
    exit(1);
}

/**
 * @brief Check wether input word is located in dictionary
 * 
 * @param word input word
 * @param wordLib dictionary to be found
 * @return true if dictionary contain word
 * @return false if dictionary not contain word
 */
bool is_lib_word(char* word, char** wordLib) {
    for (int i = 0; wordLib[i] != NULL; i++) {
        if (strcasecmp(word, wordLib[i]) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Function to process all input from commandline
 * and create usefull parameters from it
 *  
 * @param argc arguments count
 * @param argv argument vector
 * @return TerPara struct of parameters
 */
TerPara commandline_processing(int argc, char** argv) {
    TerPara parameters;
    parameters.dictionary = DICTIONARY_FILE;
    parameters.maxWords = 0;
    parameters.starterWord = NULL;
    for (++argv, --argc; argc > 0; argc--, argv++) {
        //printf("argc = %d, argv = %s\n",argc, *argv);
        if (strcmp(*argv, "--start") == 0) {
            if (parameters.maxWords != 0 
                    || parameters.starterWord != NULL 
                    || *(argv+1) == NULL) {
                error_exit_one();
            }
            parameters.starterWord = *(++argv);
            --argc;
            
            if ((strlen(parameters.starterWord) != 3 
                    && strlen(parameters.starterWord) != 4)
                    || !is_alpha(parameters.starterWord)) {
                fprintf(stderr, "uqwordiply: invalid starter word\n");
                exit(2);
            }
        } else if (strcmp(*argv, "--dictionary") == 0) {
            parameters.dictionary = *(++argv);
            if (parameters.dictionary == NULL) {
                error_exit_one();
            }
            --argc;
        } else if (strcmp(*argv, "--len") == 0) {
            if (parameters.starterWord != NULL 
                    || parameters.maxWords != 0 || *(argv+1) == NULL) {
                error_exit_one();
            }
            char* cNumber = *(++argv);
            //printf("cNumber is: %s", cNumber);
            --argc;
            if (strcmp(cNumber, "3") == 0) {
                parameters.maxWords = 3;
            } else if (strcmp(cNumber, "4") == 0) {
                parameters.maxWords = 4;
            } else {
                error_exit_one();
            }
        } else {
            error_exit_one();
        }
    }
    return parameters;
}

/**
 * @brief Printing final result including total length of words,
 * longest words found in previous guesses and dictionary
 * 
 * @param dictionary dictionary struct to pass words list and its size in
 * @param guessedWord previous guessed words
 */
void finalizing_result(Lib dictionary, char** guessedWord) {
    int totalLength = 0;
    int maxLen = 0;
    for (int i = 0; i < MAX_WORDS; i++) {
        totalLength += strlen(guessedWord[i]);
        if (maxLen < strlen(guessedWord[i])) {
            maxLen = strlen(guessedWord[i]);
        }
    }
    printf("\nTotal length of words found: %d\n"
            "Longest word(s) found:\n", totalLength);
    for (int i = 0; i < MAX_WORDS; i++) {
        if (strlen(guessedWord[i]) == maxLen) {
            printf("%s (%d)\n", guessedWord[i], (int)strlen(guessedWord[i]));
        }
    }
    printf("Longest word(s) possible:\n");
    for (int i = 0; i < dictionary.wordLibSize; i++) {
        if (strlen(dictionary.wordList[i]) == dictionary.longestLen) {
            printf("%s (%d)\n", dictionary.wordList[i], dictionary.longestLen);
        }
    }
}

/**
 * @brief Function to operating I/O and control the game flow
 * 
 * @param parameters include dictionary, starter word
 * and max word
 * @param wordLib containing list of words and its size
 * @return char** the guessed words
 */
char** game_runner(TerPara parameters, Lib wordLib) {
    char** guessedWord = (char**)malloc(sizeof(char*) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        guessedWord[i] = (char*)malloc(sizeof(char) * MAX_WORD_LENGTH);
        guessedWord[i] = "0";
    }

    printf("Welcome to UQWordiply!\n");
    printf("The starter word is: %s\n", to_upper(parameters.starterWord));
    printf("Enter words containing this word.\n");
    for (int idx = 1; idx < MAX_WORDS + 1;) {
        char* guess = (char*)malloc(sizeof(char) * MAX_WORD_LENGTH);
        //recieve input from user
        printf("Enter guess %d:\n", idx);
        guess = read_line(stdin);

        //printf("the input is: %s\n", guess); //----------------------
        
        if (strcasecmp(guess, parameters.starterWord) == 0) {
            printf("Guesses can't be the starter word - try again.\n");
            continue;
        } 
        // invalid char check
        // (guess[j] >= 65 && guess[j] <= 90) 
        //|| (guess[j] >= 97 && guess[j] <= 122)
        if (!is_alpha(guess)) {
            // These number above are ASCII code for A-Z and a-z
            printf("Guesses must contain only letters - try again.\n");
            continue;
        } 

        // starter word check
        //printf("Starter word check!\n");//----------------------

        if (strstr(to_upper(guess), to_upper(parameters.starterWord)) 
                == NULL) {
            printf("Guesses must contain the starter word - try again.\n");
            continue;
        } 
        // existed word check
        //printf("Existed word check!\n");//----------------------
        for (int i = 0; strcmp(guessedWord[i], "0"); i++) {
            if (strcmp(to_upper(guess), to_upper(guessedWord[i])) == 0) {
                printf("You’ve already guessed that word - try again.\n");
                continue;
            }   
        }
        // search word in library
        //printf("Lib search!\n");//----------------------
        if (is_lib_word(guess, wordLib.wordList)) {
            guessedWord[idx] = guess;
            idx++;
            continue;
        } else {
            printf("Guess not found in dictionary - try again.\n");
            continue;
        }
    }
    return guessedWord;
}

/**
 * Main function
*/
int main(int argc, char** argv) {
    TerPara parameters;
    parameters = commandline_processing(argc, argv);
    
    // test basic input first 
    FILE* fp = fopen(parameters.dictionary, "r");
    if (fp == NULL) {
        fprintf(stderr, "uqwordiply: dictionary file \"%s\" "
                "cannot be opened\n", parameters.dictionary);
        exit(3);
        return 0;
    }
    parameters.dictionary = NULL;
    free(parameters.dictionary);
    //printf("Dictionary read!\n");

    Lib wordLib = file_tokenize(fp, parameters.starterWord);

    //printf("word tokenized!\n");
    if (wordLib.wordList == NULL) {
        printf("This file can't be tokenized!");
        return 0;
    }
    if (parameters.starterWord == NULL) {
        //starterWord = "top";
        parameters.starterWord = 
                (char*)get_wordiply_starter_word(parameters.maxWords);
    }

    char** guessedWord = game_runner(parameters, wordLib);

    finalizing_result(wordLib, guessedWord);

    fclose(fp);
    for (int i = 0; i < MAX_WORDS; i++) {
        guessedWord[i] = NULL;
        free(guessedWord[i]);
    }
    free(guessedWord);
    free(wordLib.wordList);
    return 0;
}