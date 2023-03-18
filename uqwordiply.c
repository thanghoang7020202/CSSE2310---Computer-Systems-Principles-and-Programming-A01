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

typedef struct WordLibrary {
    char** wordList;
    int wordLibSize;
    int longestLen;
} Lib;

typedef struct TerminalParameter {
    char* dictionary;
    char* starterWord;
    int maxWords;
}TerPara;

/* is_alpha()
 * ---------------
 * Check whether the word containing all letters
 * 
 * word: word to check
 * 
 * Returns: true if word contain only letters and false
 *      if word contain any character that not a letter
 */
bool is_alpha(char* word) {
    for (int i = 0; i < strlen(word); i++) {
        if (isalpha(word[i]) == 0) {
            return false;
        }
    }
    return true;
}

/* read_line()
 * ---------------
 * read each line from file stream
 * 
 * stream: file stream to be read
 * 
 * Returns: char* buffer of read words and NULL if
 *      stream is at eof or some words was guessed already 
 * Errors: exit statue of 4 if meet EOF
 */
char* read_line(FILE* stream, Lib guessedWord) {
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
            if (guessedWord.wordLibSize > 0) {
                return NULL;
            }
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

/* to_lower()
 * ---------------
 * change all word input to lower case
 * 
 * word: word input word
 * 
 * Returns: char* lower cased word
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

/* to_upper()
 * ---------------
 * change all word input to upper case
 * 
 * word: input word
 * 
 * Returns: char* upper cased word
 */
char* to_upper(char* word) {
    if(word == NULL) return NULL;
    for (int i = 0; i < strlen(word); i++) {
        word[i] = toupper(word[i]);
    }
    return word;
}

/* add()
 * ---------------
 * Add line to a list (array)
 * 
 * wordLib: array to add in
 * line: words to add in array
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

/* file_tokenize()
 * ---------------
 * separate each word in library 
 * and put them in a list (array)
 * 
 * fp: file point to read
 * starterWor:d inital word
 * 
 * Returns: Lib dictionary struct contain separated word
 */
Lib file_tokenize(FILE* fp, TerPara parameters) {
    Lib wordLib;
    wordLib.wordList = NULL;
    wordLib.wordLibSize = 0;
    wordLib.longestLen = 0;
    char line[MAX_WORD_LENGTH];
    
    for (; fgets(line, MAX_WORD_LENGTH, fp);) {
        int lengthLine = strlen(line);
        // replace newline with NULL terminator
        line[lengthLine - 1] = '\0';
        if (!is_alpha(line) || strstr(to_upper(line),
                to_upper(parameters.starterWord)) == NULL){
            continue;
        }
        add(&wordLib, line);
    }
    return wordLib;
}

/* error_exit_one()
 * ---------------
 * function to exit 1 and print structure of commandline input
 */
void error_exit_one() {
    fprintf(stderr, "Usage: uqwordiply [--start starter-word | --len length]"
            " [--dictionary filename]\n");
    exit(1);
}

/* is_lib_word()
 * ---------------
 * Check wether input word is located in dictionary
 * 
 * word: input word
 * wordLib: dictionary to be found
 * 
 * Returns: true if dictionary contain word 
 *      and false if dictionary not contain word
 */
bool is_lib_word(char* word, Lib wordLib) {
    for (int i = 0; i < wordLib.wordLibSize; i++) {
        //printf("compare between %s and %s\n", word, wordLib.wordList[i]);
        if (strcasecmp(word, wordLib.wordList[i]) == 0) {
            return true;
        }
    }
    return false;
}

/* commandline_processing()
 * ---------------
 * Function to process all input from commandline
 * and create usefull parameters from it
 *  
 * argc: arguments count
 * argv: argument vector
 * 
 * Returns: TerPara struct of parameters
 * Errors: exit with statue 1 if recieve invalid argument,
 *      exit with statue 2 if there is an invalid starter word
 * 
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

/* game_runner()
 * ---------------
 * Function to operating I/O and control the game flow,
 *      make recommendation about user input.
 * 
 * parameters: include dictionary, starter word
 *      and max word
 * wordLib: containing list of words and its size
 * 
 * Returns: char** the guessed words
 */
Lib game_runner(TerPara parameters, Lib wordLib, Lib guessedWord) {
    printf("Welcome to UQWordiply!\n");
    printf("The starter word is: %s\n", to_upper(parameters.starterWord));
    printf("Enter words containing this word.\n");
    for (int idx = 1; idx < MAX_WORDS + 1;) {
        char* guess = (char*)malloc(sizeof(char) * MAX_WORD_LENGTH);
        //recieve input from user
        printf("Enter guess %d:\n", idx);
        guess = read_line(stdin, guessedWord);
        if (guess == NULL) {
            return guessedWord;
        }
        if (strcasecmp(guess, parameters.starterWord) == 0) {
            printf("Guesses can't be the starter word - try again.\n");
            continue;
        } 
        if (!is_alpha(guess)) { // invalid char check
            printf("Guesses must contain only letters - try again.\n");
            continue;
        } 
        // starter word check
        if (strstr(to_upper(guess), to_upper(parameters.starterWord)) 
                == NULL) {
            printf("Guesses must contain the starter word - try again.\n");
            continue;
        } 
        // flag for existed word check (0 if word have not guessed)
        int flag = 0; 
        for (int i = 0; i < guessedWord.wordLibSize; i++) {
            if (strcmp(to_upper(guess), to_upper(guessedWord.wordList[i])) 
                    == 0) {
                printf("You've already guessed that word - try again.\n");
                flag = 1;
                break;
            }   
        }
        if (flag == 1) {
            continue;
        }
        // search word in library
        if (is_lib_word(guess, wordLib)) {
            guessedWord.wordList[idx - 1] = guess;
            guessedWord.wordLibSize += 1;
            idx++;
        } else {
            printf("Guess not found in dictionary - try again.\n");
        }
    }
    return guessedWord;
}

/* finalizing_result()
 * ---------------
 * Printing final result including total length of words,
 * longest words found in previous guesses and dictionary
 * 
 * dictionary: dictionary struct to pass words list and its size in
 * guessedWord: previous guessed words
 */
void finalizing_result(Lib dictionary, Lib guessedWord) {
    int totalLength = 0;
    int maxLen = 0;
    for (int i = 0; i < guessedWord.wordLibSize; i++) {
        totalLength += strlen(guessedWord.wordList[i]);
        if (maxLen < strlen(guessedWord.wordList[i])) {
            maxLen = strlen(guessedWord.wordList[i]);
        }
    }
    printf("\nTotal length of words found: %d\n"
            "Longest word(s) found:\n", totalLength);
    for (int i = 0; i < guessedWord.wordLibSize; i++) {
        if (strlen(guessedWord.wordList[i]) == maxLen) {
            printf("%s (%d)\n", guessedWord.wordList[i],
                    (int)strlen(guessedWord.wordList[i]));
        }
    }
    printf("Longest word(s) possible:\n");
    for (int i = 0; i < dictionary.wordLibSize; i++) {
        if (strlen(dictionary.wordList[i]) == dictionary.longestLen) {
            printf("%s (%d)\n", dictionary.wordList[i], dictionary.longestLen);
        }
    }
}

/* main()
 * ---------------
 * main function that initialize and call all functions
 * 
 * argc: argument count
 * argv: argument vector
 * 
 * Returns: int 0 if function works normally
 * Errors: Exit with statue of 3 if find can not be open.
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

    if (parameters.starterWord == NULL) {
        parameters.starterWord = 
                (char*)get_wordiply_starter_word(parameters.maxWords);
    }

    Lib wordLib = file_tokenize(fp, parameters);

    if (wordLib.wordList == NULL) {
        printf("This file can't be tokenized!");
        return 0;
    }

    Lib guessedWord;
    guessedWord.wordList = (char**)malloc(sizeof(char*) * MAX_WORDS);
    for (int i = 0; i < MAX_WORDS; i++) {
        guessedWord.wordList[i] = 
                (char*)malloc(sizeof(char) * MAX_WORD_LENGTH);
        guessedWord.wordList[i] = "0";
    }
    guessedWord.wordLibSize = 0;
    guessedWord = game_runner(parameters, wordLib, guessedWord);
    finalizing_result(wordLib, guessedWord);
    fclose(fp);
    for (int i = 0; i < MAX_WORDS; i++) {
        guessedWord.wordList[i] = NULL;
        free(guessedWord.wordList[i]);
    }
    free(guessedWord.wordList);
    free(wordLib.wordList);
    return 0;
}