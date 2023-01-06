//
// Created by Cata on 2/25/2020.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tinyHelper.h"

/**
 * Adds a single card to to the cards array
 * Yes its easy to do but it is more readable this way I think
 */
void addCard(char card, char *cards) {
    cards[(unsigned) card]++;
}

/**
* Prints an error message including line number and nice formatting beforehand
*/
void complain(unsigned *line, char* message, ...) {
    va_list args;
    va_start(args, message);

    int length = snprintf(NULL, 0, "Error on line %d: %s\n ",*line, message);
    char errorMessage[length];
    snprintf(errorMessage, length, "Error on line %d: %s\n ",*line, message);

    vfprintf(stderr,errorMessage, args);
    va_end(args);

    exit(1);
}

/**
 * Returns the value of a card, as a number between 0 and 12 (inclusive)
 */
char inline getValue(char card) {
    return card%13;
}

/**
 * Returns the suit of a card, as a number between 0 and 3 (inclusive)
 * The order of suits is clubs, spades, diamonds, hearts
 */
char inline getSuit(char card) {
    return card/13;
}

/**
 * Adds the card specified and all smaller cards of the same suit
 * Input card must be a number between 0 and 51, inclusive
 */
void fillFoundation(char card, char *cards) {
    do {
        addCard(card, cards);
    } while (getValue(card)>0 && (card-- || 1)); //If we just added an ace, terminate. Otherwise, add the next lowest card.
}

/**
 * Gets the index for the start of the stock where | == -1
 * @param wasteStock
 * @return
 */
unsigned inline getBeginningOfStock(char *wasteStock){
    unsigned i = 0;
    while (wasteStock[i] != -1) i++;
    return i;
}

/**
 * Takes a number between 0 and 3 and gives the suit character associated with it.
 * If the number is invalid, crashes. This is because the programmer controls which numbers this is called for so this is ALWAYS an error
 */
char formatNumToSuit(char suit) {
    switch (suit) {
        case 0: return 'c';
        case 1: return 's';
        case 2: return 'd';
        case 3: return 'h';
        default: 	fprintf(stderr,"PROGRAMMER ERROR! IN FORMATNUMTOSUIT %d\n", suit); //This should be unreachable even with bad input
            exit(2);
    }
}

/**
 * Takes a number between 0 and 12 and gives the rank character associated with it.
 * If the number is invalid, crashes. This is because the programmer controls which numbers this is called for so this is ALWAYS an error
 */
char formatNumToRank(char rank) {
    if (1<=rank && rank < 9) return rank+'1';
    switch(rank) {
        case 0: return 'A';
        case 9: return 'T';
        case 10: return 'J';
        case 11: return 'Q';
        case 12: return 'K';
        default: 	fprintf(stderr,"PROGRAMMER ERROR! IN FORMATNUMTORANK %d\n", rank); //This should be unreachable even with bad input
            exit(2);
    }
}

/**
 * Takes a character c, s, d, or h, and gives the suit value associated with it.
 * If the input is not a suit char, returns -1
 */
char formatSuitToNum(char suit) {
    if ('c' ==suit) return 0;
    else if ('s' ==suit) return 1;
    else if ('d' ==suit) return 2;
    else if ('h' ==suit) return 3;
    else return -1;
}

/**
 * Takes a character A, 2, 3 ... 8, 9, T, J, Q, K, and gives the rank value associated with it.
 * If the input is not a rank char, returns -1
 */
char formatRankToNum(char rank) {
    if ('A'==rank) return 0;
    else if ('2'<=rank && '9'>=rank) return rank-'1';
    else if ('T'==rank) return 9;
    else if ('J'==rank) return 10;
    else if ('Q'==rank) return 11;
    else if ('K'==rank) return 12;
    else return -1;
}

void fprintCard(char card, FILE* outfile, char *prefix, char *suffix) {
    fprintf(outfile, "%s%c%c%s", prefix, formatNumToRank(getValue(card)), formatNumToSuit(getSuit(card)), suffix);
}

/**
 * Returns the index of the card at the bottom of the tableau column
 */
unsigned char inline getBottomColumn(char *tableau, char column) {
    unsigned char retval = (6-column)*21; //guaranteed not to be -2
    while(tableau[retval]!=-2)retval++;
    return retval-1;
}

/**
 * Returns the index of the card at the top of the waste
 */
//char getTopWaste(char *wasteStock) {
//    char i;
//    for (i=0; -1 != wasteStock[i]; i++);
//    return i-1;
//}

/**
 * Removes the card at the top of the waste from it, returning the card that was removed
 */
char inline removeTopWaste(char *wasteStock) {
    char i = getBeginningOfStock(wasteStock) - 1;
    char retval = wasteStock[(unsigned) i];
    for (; -2 != wasteStock[(unsigned) i]; i++) wasteStock[(unsigned) i] = wasteStock[(unsigned) i+1];
    return retval;
}

/**
 * Set the card at the end of the specified to the given card
 */
void setBottomColumn(char *tableau, char column, char card) {
    unsigned char index = getBottomColumn(tableau, column);
    tableau[index+1]=card;
    tableau[index+2]=-2;
}

/**
 * Shows the current values in the different arrays:
 * Foundation, Tableau, and WasteStock
 * Can be used for other arrays that use digits/char
 */
void show_array(char *arr, int len, char *preface) {
    int i;
    for (i=0;i<len;i++) {
        if (i) printf(", "); else printf("%s: [",preface);
        printf("%3d",arr[i]);
    }
    printf("]\n");
}

/**
 * Swaps card locations
 * @param arr
 * @param num
 */
void inline swapCard(char *arr, unsigned num){
    char cardTemp = arr[num];
    arr[num] = arr[num - 1];
    arr[num - 1] = cardTemp;
}

