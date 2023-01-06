//
// Created by Cata on 2/27/2020.
//

#include <stdio.h>
#include <stdlib.h>

#include "simplicity.h"
#include "tinyHelper.h"
#include "thiccHelper.h"

//#define DEBUG_PRINT

/*
* Checks the cards array to make sure the cards given
* have been legal
*/
void checkCards(char *cards) {
    unsigned char i, dupes = 0, miss = 0;
    char duplicates[52], missing[52];
    for (i=0; i<52; i++){ //fixed not detecting that the King of Hearts was missing.
        if (cards[i] > 1) {
            duplicates[dupes++] = i;
        } else if(cards[i] == 0){
            missing[miss++] = i;
        }
    }
    if (dupes > 0){
        fprintf(stderr, "Duplicate cards: ");
        for (i = 0; i<dupes; i++) {
            fprintf(stderr, "%c%c ", formatNumToRank(getValue(duplicates[i])), formatNumToSuit(getSuit(duplicates[i])));
        }
        fprintf(stderr, "\n");
        exit(1);
    }
    if (miss > 0) {
        fprintf(stderr, "Missing cards: ");
        for (i = 0; i < miss; i++){
            fprintf(stderr, "%c%c ", formatNumToRank(getValue(missing[i])), formatNumToSuit(getSuit(missing[i])));
        }
        fprintf(stderr, "\n");
        exit(1);
    }
}

/*
 * Completely sad this is not what I wanted to do
 */
void currentlyWorksPrintingTableauArrayButNOTWhatIWant(char *arr, unsigned len){
    int i, count = 0, maxCount = 0;
    if (len == 147) {
        for(i = 0; i < len; i++){
            if (arr[i*21] != -1){
                while (arr[i] != -1 && i < len){
                    if (arr[i]) {
                        printf("## ");
                        count++;
                    }
                    i++;
                }
            }
            while (arr[i] != -2 && i < len){
                if (arr[i] == -1){
                    i++;
                }
                if (arr[i] == -2){
                    break;
                }
                printf("%c%c ", formatNumToRank(getValue(arr[i])), formatNumToSuit(getSuit(arr[i])));
                i++;
                count++;
            }
            if(count > maxCount){
                maxCount = count;
            }
            count = 0;
            printf("\n");
        }
    }
}

/*
 * Prints the tableau in good formatting to outfile
 */
void tableauPrinting(char *tableau, FILE* outfile) {
    char position[7] = {0};
    char i, j=0, done=0;
    while (!done){
        done = 1;
        //write a single row of the tableau
        for(i = 6; i >= 0; i--) {
            //check what is at the array position IF WE ARE NOT ALREADY EMPTY FOR SURE - SINISTER BUG
            if(2 == position[(unsigned char)i]) continue;
            char card = tableau[i * 21 + position[(unsigned char)i] + j];
            if (card == -1) {
                position[(unsigned char)i] = 1;
                card = tableau[i * 21 + position[(unsigned char)i] + j];
            }
            if (card == -2) {
                position[(unsigned char)i] = 2;
            }
            if (position[(unsigned char)i] != 2){
                done = 0;
            }
        }
        if (!done) {
            for (i = 6; i >= 0; i--) {
                char card;
                card = tableau[i*21 + position[(unsigned char)i] +j];
                if (position[(unsigned char)i] == 0) {
                    fprintf(outfile,"## ");
                } else if (position[(unsigned char)i] == 1) {
                    fprintf(outfile,"%c%c ", formatNumToRank(getValue(card)), formatNumToSuit(getSuit(card)));
                } else if (position[(unsigned char)i] == 2) {
                    fprintf(outfile,".. ");
                } else {fprintf(stderr, "PROGRAMMER ERROR IN TABLEAU PRINTING\n");exit(1);}
                if (!i) fprintf(outfile, "\n");
            }
        }
        j++;
    }
}

void printTableauLineExchange(char *tableau, FILE* outfile, char line, char size) {
    int i=0;
    if (size != 54) fprintf(outfile, "  ");
    while (-2 != tableau[line*21+i]) {
        char card = tableau[line*21+i];
        if (card == -1) fprintf(outfile, "| ");
        else fprintf(outfile, "%c%c%s", formatNumToRank(getValue(card)), formatNumToSuit(getSuit(card)),  " ");
        i++;
    }
    fprintf(outfile, "\n");
}

/**
 * Prints, to outfile, the passed in card configuration, in exchange format if specified.
 */
void printConfiguration(struct gamestate *data, struct parserConfig *config) {
#ifdef DEBUG_PRINT
    show_array(data->wasteStock,54, "Waste and Stock");
        show_array(data->foundations,4,"Foundations");
        int i;
        for (i = 0; i<7; i++) show_array(data->tableau+i*21,21,"Tableau line");
	    tableauPrinting(data->tableau, stdout);
#endif
    if (config->isExchange) {
        fprintf(config->outFile,"RULES:\n  turn %d\n", data->turn);
        (-1 == data->limit) ? fprintf(config->outFile,"  unlimited\n"):fprintf(config->outFile,"  limit %d\n",data->limit);

        fprintf(config->outFile,"FOUNDATIONS:\n");
        if (data->foundations[(unsigned) formatSuitToNum('c')] == -1) fprintf(config->outFile, "  _c\n");
        else fprintCard(data->foundations[(unsigned) formatSuitToNum('c')], config->outFile, "  ","\n");
        if (data->foundations[(unsigned) formatSuitToNum('d')] == -1) fprintf(config->outFile, "  _d\n");
        else fprintCard(data->foundations[(unsigned) formatSuitToNum('d')] + 26, config->outFile, "  ","\n");
        if (data->foundations[(unsigned) formatSuitToNum('h')] == -1) fprintf(config->outFile, "  _h\n");
        else fprintCard(data->foundations[(unsigned) formatSuitToNum('h')] + 39, config->outFile, "  ","\n");
        if (data->foundations[(unsigned) formatSuitToNum('s')] == -1) fprintf(config->outFile, "  _s\n");
        else fprintCard(data->foundations[(unsigned) formatSuitToNum('s')] + 13, config->outFile,"  ", "\n");

        fprintf(config->outFile,"TABLEAU:\n");
        int i; for (i=0;i<=6;i++) printTableauLineExchange(data->tableau, config->outFile, i, 0);

        fprintf(config->outFile, "STOCK:\n");
        printTableauLineExchange(data->wasteStock,config->outFile, 0, 54);

        fprintf(config->outFile,"MOVES:\n");
    } else {
        fprintf(config->outFile, "Foundations\n");
        if (data->foundations[(unsigned) formatSuitToNum('c')] == -1) fprintf(config->outFile, "_c ");
        else fprintCard(data->foundations[(unsigned) formatSuitToNum('c')],config->outFile,""," ");
        if (data->foundations[(unsigned) formatSuitToNum('d')] == -1) fprintf(config->outFile, "_d ");
        else fprintCard((data->foundations[(unsigned) formatSuitToNum('d')] + 26),config->outFile,""," ");
        if (data->foundations[(unsigned) formatSuitToNum('h')] == -1) fprintf(config->outFile, "_h ");
        else fprintCard((data->foundations[(unsigned) formatSuitToNum('h')] + 39),config->outFile,""," ");
        if (data->foundations[1] == -1) fprintf(config->outFile, "_s\n");
        else fprintCard((data->foundations[(unsigned) formatSuitToNum('s')] + 13), config->outFile,"", "\n");

        fprintf(config->outFile,"Tableau\n");
        tableauPrinting(data->tableau, config->outFile);

        fprintf(config->outFile,"Waste top\n");

        char wasteTopIndex, i;
        for (wasteTopIndex=0; -1 != data->wasteStock[(unsigned) wasteTopIndex];wasteTopIndex++);
        if (!wasteTopIndex) fprintf(config->outFile, "(empty)\n");
        else {
            wasteTopIndex-=data->turn;
            if (wasteTopIndex<0) wasteTopIndex=0;

            for (i=0; i<data->turn && -1!=data->wasteStock[(unsigned)wasteTopIndex+i];i++) {
                char wasteTop = data->wasteStock[(unsigned) wasteTopIndex+i];
                fprintCard(wasteTop,config->outFile,"", " ");
            }
            printf("\n");
        }
    }
}
