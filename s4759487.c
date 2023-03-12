#include <stdio.h>
#include <stdlib.h>
//#include <csse2310a1.h>
#include "support.h"

#define MAX_WORD_LENGTH 50
#define MAX_WORDS 5

int main(int argc, char* argv) {

    // test basic input first
    char starterWord[MAX_WORD_LENGTH] = "TOP";
    // open dictionary file, fix path later
    char* dictionary = ".\\dictionary.txt"; 
    char** guessedWord = (char**)malloc(sizeof(char*)*MAX_WORDS);
    for (int i = 0 ; i < MAX_WORDS; i++) {
        guessedWord[i] = (char*)malloc(sizeof(char)*MAX_WORD_LENGTH);
        guessedWord[i] = "0";
    }
    FILE* fp = fopen(dictionary, "r");

    if(fp == NULL) {
        printf("uqwordply: dictionary file \"%s\" cannot be opened\n", dictionary);
        exit(3);
        return 0;
    }

    char** wordLib = filetokenize(fp);

    printf("Wellcome to UQWordiply!\nThe starter word is: %s", starterWord);
    printf("Enter words containing this word.\n");
    for (int i = 0; i < MAX_WORDS;) {
        char* guest1 = (char*)malloc(sizeof(char)*MAX_WORD_LENGTH);
        scanf("Enter guest %d: %s", i,guest1);
        size_t len = strlen(guest1);
        for (int j = 0; j < len; j++) {
            //recieve input from user
            scanf("Enter guest %d: %s", i,guest1);
            size_t len = strlen(guest1);
            
            if (strcmp(guest1,EOF) == 0) {
                exit(4);
            }
            if ((guest1[j] >= 65 && guest1[j] <= 90) || (guest1[j] >= 97 && guest1[j] <= 122)) {
                // These number above are ASCII code for A-Z and a-z
                printf("Guesses must contain only letters - try again.\n");
            } else if (strcmp(guest1,starterWord) == 0) {
                printf("Guesses can't be the starter word - try again.\n");
            } else if (strstr(guest1,starterWord) == NULL) {
                printf("Guesses must contain the starter word - try again.\n");
            } else {
                for(int i = 0; guessedWord[i] == "0"; i++) {
                    if (strcmp(guest1,guessedWord[i]) == 0) {
                        printf("You already guessed that word - try again.\n");
                    } else {
                            for(int j = 0; wordLib[j] == "0"; j++) {
                                if (strcmp(guest1,wordLib[i]) == 0) {
                                printf("Guess not found in dictionary - try again.\n");
                                } else {
                                    guessedWord[i] = guest1;
                                    i++;
                                    wordLib[j] = "0"; // remove the word from the wordLib
                                }
                            }
                    }
                }
            }
        }
        free(guest1);
    }
    free(guessedWord);
    free(wordLib);
    free(fp);
    free(starterWord);
    
    free(dictionary);
    return 0;
}