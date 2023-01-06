//
// Created by Cata on 2/25/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyHelper.h"
#include "smallHelper.h"

/*
 * Reads from readfile to see if the next characters are word. If so, return 1. Otherwise, return 0.
 * In the event that it was not, it ungets the last character that it read. (This is useful for distinguishing unlimited vs limit N for example)
 */
char assertNextWord(FILE *readFile, unsigned *line, char* word) {
    unsigned i;
    for (i=0; word[i]; i++) {
        int c = fgetc(readFile);
        if (EOF==c) {
            int length = snprintf(NULL, 0, "Expected %s but file ended unexpectedly. ",word); //Get the length of our error message, ending in space for null terminator
            char fileIOException[length];
            snprintf(fileIOException, length, "Expected %s but file ended unexpectedly.", word);
            complain(line, fileIOException);
        }
        if ('\n'==c || '\r' == c) (*line)++;
        if (c != word[i]) {
            ungetc(c, readFile);
            return 0;
        }
    }
    return 1;
}

/*
 * Reads from readfile to get the next positive integer value from it. Reads six characters or until the first non-integer character, then returns the value.
 * Default if called and there is no number is to crash.
 */
int getNextInt(FILE *readFile, unsigned *line) {
    char c = fgetc(readFile);
    if (c > '9' || c < '0') complain(line, "Expected a number but none was found.");
    int returnValue = c-'0';
    unsigned i;
    for (i = 0; i < 5; i++) {
        char c = fgetc(readFile);
        if (EOF==c) complain(line, "Expected a number but file ended unexpectedly.");
        if ('0'<=c && '9'>=c) {
            returnValue = returnValue*10+c-'0';
        } else {
            ungetc(c, readFile);
        }
    }
    return returnValue;
}

/*
 * Reads two characters from readFile that must be formatted as a card (per the specification) and outputs that card as an integer from 0 to 51.
 * Card 0 is the Ace of clubs, card 1 is the 2 of clubs... card 12 is the king of clubs, card 13 is the ace of spades...
 * Card 26 is the ace of diamonds, card 39 is the ace of hearts
 */
char getNextCard(FILE *readFile, unsigned *line) {
    char rank = fgetc(readFile);
    if (EOF == rank) complain(line, "Expected a card's rank but file ended unexpectedly.");
    char suit = fgetc(readFile);
    if (EOF == suit) complain(line, "Expected a card's suit but file ended unexpectedly.");

    rank = formatRankToNum(rank);
    suit = formatSuitToNum(suit);
    if (-1 == rank) complain(line, "Illegal rank value for a card.");
    if (-1 == suit) complain(line, "Illegal suit value for a card.");

    //All black cards are less than 26, all red cards are greater than or equal to 26
    //The order of suits is clubs, spades, diamonds, hearts
    //The biggest card is therefore king of hearts with value 51

    return rank + (suit*13);
}

/*
 * Prints the cards with value and suit
 */
void printCards(char *cards) {
    printf("[");
    unsigned i;
    for (i = 0; i<52; i++) {
        if (!getValue(i)%13) {
            switch(getSuit(i)) {
                case 0: printf("Clubs: ");	break;
                case 1: printf("  Spades: ");	break;
                case 2: printf("  Diamonds: ");	break;
                case 3: printf("  Hearts: ");	break;
            }
        }
        else if (i) {
            printf(",");
        }
        printf("%d",cards[i]);
    }
    printf("]");
}

