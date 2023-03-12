#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <csse2310a1.h>
#include "support.h"

#define MAX_WORD_LENGTH 50
#define MAX_WORDS 5

int main(int argc, char** argv) {

    // test basic input first
    char starterWord[MAX_WORD_LENGTH] = "TOP";
    // open dictionary file, fix path later
    char* dictionary = "./dictionary.txt"; 
    char** guessedWord = (char**)malloc(sizeof(char*)*MAX_WORDS);
    for (int i = 0 ; i < MAX_WORDS; i++) {
        guessedWord[i] = (char*)malloc(sizeof(char)*MAX_WORD_LENGTH);
        //why can't I use guessedWord[i] = "0"; here? it lead to free memory error!
        //guessedWord[i] = "0";
    }
    FILE* fp = fopen(dictionary, "r");

    if(fp == NULL) {
        printf("uqwordply: dictionary file \"%s\" cannot be opened\n", dictionary);
        exit(3);
        return 0;
    }
    
    printf("Hello World!");

    char** wordLib = filetokenize(fp);

    if (wordLib == NULL) {
        printf("This file can't be tokenized!");
        return 0;
    }

    printf("Wellcome to UQWordiply!\nThe starter word is: %s", starterWord);
    printf("Enter words containing this word.\n");
    for (int i = 0; i < MAX_WORDS;) {
        char guess[60];
        //recieve input from user
        scanf("Enter guest %d: %s", &i,guess);
        size_t len = strlen(guess);
        // checking process
        if (strcmp(guess,EOF) == 0) {
                exit(4);
                }
        if (strcmp(guess,starterWord) == 0) {
            printf("Guesses can't be the starter word - try again.\n");
        } 
        // run though each character in the guess
        for (int j = 0; j < len; j++) {
            if ((guess[j] >= 65 && guess[j] <= 90) || (guess[j] >= 97 && guess[j] <= 122)) {
                // These number above are ASCII code for A-Z and a-z
                printf("Guesses must contain only letters - try again.\n");
            } else if (strstr(guess,starterWord) == NULL) {
                printf("Guesses must contain the starter word - try again.\n");
            } else {
                for(int i = 0; guessedWord[i] == "0"; i++) {
                    if (strcmp(guess,guessedWord[i]) == 0) {
                        printf("You already guessed that word - try again.\n");
                    } else {
                            for(int j = 0; wordLib[j] == "0"; j++) {
                                if (strcmp(guess,wordLib[i]) == 0) {
                                printf("Guess not found in dictionary - try again.\n");
                                } else {
                                    guessedWord[i] = guess;
                                    i++;
                                    wordLib[j] = "0"; // remove the word from the wordLib
                                }
                            }
                    }
                }
            }
        }
        free(guess);
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