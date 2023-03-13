#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <csse2310a1.h>
#include "support.h"

int main(int argc, char** argv) {
    char* dictionary = DICTIONARY_FILE;
    char* starterWord = NULL;
    int maxWords = 0;
    for(++argv, --argc; argc > 0 ; argc --, argv++) {
        printf("argc = %d, argv = %s\n",argc, *argv);
        if(strcmp(*argv, "--start") == 0) {
            if(maxWords != 0) {
                errorExitOne();
            }
            starterWord = *(++argv);
            --argc;
            printf ("startword is: %s\n", starterWord);
            if (strlen(starterWord) != 3) {
                fprintf(stderr, "uqwordiply: invalid starter word\n");
                exit(2);
            }
        } else if (strcmp(*argv, "--dictionary") == 0) {
            dictionary = *(++argv);
            --argc;
        } else if (strcmp(*argv , "--len") == 0) {
            if (starterWord != NULL) {
                errorExitOne();
            }
            maxWords = (*(++argv)) - '0';
            --argc;
            if (maxWords != 3 && maxWords != 4) {
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
        //guessedWord[i] = "0";
    }
    FILE* fp = fopen(dictionary, "r");
    
    if(fp == NULL) {
        printf("uqwordply: dictionary file \"%s\" cannot be opened\n", dictionary);
        exit(3);
        return 0;
    }
    
    char** wordLib = filetokenize(fp);

    if (wordLib == NULL) {
        printf("This file can't be tokenized!");
        return 0;
    }
    
    if (starterWord == NULL) {
        int i = 0;
        while (1) {
            int random = rand() % 7;
            starterWord = wordLib[random];
            break;
        }
        // get_wordiply_starter_word();
    }
    printf("Wellcome to UQWordiply!\nThe starter word is: %s", starterWord);
    printf("Enter words containing this word.\n");
    for (int i = 0; i < MAX_WORDS;) {
        char guess[60];
        //recieve input from user
        scanf("Enter guest %d: %s", &i, guess);
        size_t len = strlen(guess);
        // checking process
        if (feof(stdin)) {
                exit(4);
        }
        if (strcmp(guess,starterWord) == 0) {
            printf("Guesses can't be the starter word - try again.\n");
            continue;
        } 
        // invalid char check
        for (int j = 0; j < len; j++) {
            if ((guess[j] >= 65 && guess[j] <= 90) || (guess[j] >= 97 && guess[j] <= 122)) {
                // These number above are ASCII code for A-Z and a-z
                printf("Guesses must contain only letters - try again.\n");
                continue;
            } 
        }
        // starter word check
        if (strstr(guess,starterWord) == NULL) {
                printf("Guesses must contain the starter word - try again.\n");
                continue;
        } 

        // existed word check
        for(int i = 0; strcmp(guessedWord[i],"0"); i++) {
            if (strcmp(guess,guessedWord[i]) == 0) {
                printf("You already guessed that word - try again.\n");
                continue;
            }   
        }
        // search word in library
        for(int j = 0; strcmp(wordLib[j],"0"); j++) {
            if (strcmp(guess,wordLib[i]) != 0) {
                guessedWord[i] = guess;
                i++;
                wordLib[j] = "0"; // remove the word from the wordLib
                continue;
            }
        }

        printf("Guess not found in dictionary - try again.\n");
        
    }
    
    fclose(fp);
    for (int i =0; i < MAX_WORDS; i++) {
        free(guessedWord[i]);
    }
    free(guessedWord);
    free(dictionary);
    free(wordLib);
    return 0;
}