//
// Created by Cata on 2/16/2020.
//
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "moves.h"
#include "tinyHelper.h"
#include "thiccHelper.h"
#include "simplicity.h"

//#define DEBUG_PRINT


/*
 * File to develop list of methods to move cards based on each scenario of move
 */

/**
 * Checks the cards in the waste to see if top card can be moved to foundation.
 * Aces can be moved right away, otherwise higher rank may be placed on card with one lower rank
 */
void moveWasteToFoundation(struct gamestate *data, struct parserConfig *config) {
    char index = getBeginningOfStock(data->wasteStock) - 1;
    if (index == -1) moveError(data, config, "w->f");
    char rank = getValue(data->wasteStock[(int) index]);
    char suit = getSuit(data->wasteStock[(int) index]);
    if (getValue(data->foundations[(unsigned) suit]) != rank-1) moveError(data, config, "w->f");
    data->foundations[(unsigned) suit]++;
    removeTopWaste(data->wasteStock);
    data->numWaste--;
}

/**
 *Check and move top waste card to tableau.
 * If King, can be placed on empty column, otherwise placed on bottom card
 * if waste card is of lower rank and opposite color
 * ColumnTo ranges from 0 to 6
 */
void moveWasteToTableau(struct gamestate *data, struct parserConfig *config, char columnTo) {
    //could use the method but that would be less efficient
    char topCard = getBeginningOfStock(data->wasteStock) - 1;
    unsigned char indexTo = getBottomColumn(data->tableau, columnTo);
    if (topCard == -1) moveError(data, config, "w->%c", columnTo+'1');

    char rankWaste = getValue(data->wasteStock[(unsigned) topCard]);
    char suitWaste = getSuit(data->wasteStock[(unsigned) topCard]);
    char rankTableau = getValue(data->tableau[(unsigned) indexTo]);
    char suitTableau = getSuit(data->tableau[(unsigned) indexTo]);

    if (rankWaste == 12 && data->tableau[(unsigned) indexTo] == -1 && data->tableau[(unsigned) indexTo+1] == -2){
        data->tableau[(unsigned) indexTo+1] = removeTopWaste(data->wasteStock);
    } else if (data->tableau[(unsigned) indexTo] != -1 && rankWaste == rankTableau-1 && suitWaste/2 != suitTableau/2){
        data->tableau[(unsigned) indexTo+1] = removeTopWaste(data->wasteStock);
    } else moveError(data, config, "w->%c", columnTo+'1');

    data->tableau[indexTo+2] = -2;
    data->numWaste--;
}

/**
 * Check and move ascending order of cards from Tableau to foundation
 * if bottom card is an Ace move it to appropriate foundation spot,
 * otherwise the bottom card of tableau may be placed on top of card in foundation
 * with one rank lower in the same suit.
 */
void moveTableauToFoundation(struct gamestate *data, struct parserConfig *config, char columnFrom) {
    //if (!canMoveTableauToFoundation(data, config, columnFrom)) moveError(data, config, "%c->f", columnFrom+'1'); //Not using this for speed reasons, would re-calculate a bunch of stuff

    unsigned char indexFrom = getBottomColumn(data->tableau, columnFrom);
    if (-1 == data->tableau[indexFrom]) moveError(data, config, "%c->f", columnFrom+'1');
    char rank = getValue(data->tableau[indexFrom]);
    char suit = getSuit(data->tableau[indexFrom]);
    if (getValue(data->foundations[(unsigned) suit] != rank-1)) moveError(data, config, "%c->f", columnFrom+'1');

    //now add the thing to the foundations
    data->foundations[(unsigned) suit]++;
    data->tableau[indexFrom]=-2;
    if (indexFrom%21 > 1 && -1 == data->tableau[indexFrom-1]) {
        swapCard(data->tableau, indexFrom-1);
        data->numCovered--;
    }
}

/**
 * Moves a pile of uncovered cards from one column to another.
 * If top uncovered card in pile is a King can be moved to an empty column,
 * otherwise top uncovered card can be placed on another card/pile if bottom card
 * of pile is one rank higher and opposite suit.
 */
void moveTableauColumn(struct gamestate *data, struct parserConfig *config, char columnTo, char columnFrom) {
    //if (!canMoveTableauColumn(data, config,columnTo,columnFrom)) moveError(data, config, "%c->%c",columnFrom+'1',columnTo+'1'); //Not using this for speed reasons, would re-calculate a bunch of stuff

    unsigned char indexTo = getBottomColumn(data->tableau, columnTo);
    unsigned char indexFrom = getBottomColumn(data->tableau, columnFrom);

    char toRank = getValue(data->tableau[indexTo]);
    if (-1==data->tableau[indexTo]) toRank=13;

    //move indexFrom up through its column until we find -1 or we find a match
    while (data->tableau[indexFrom]!=-1) {
        //if rank is one less, and suit is opposite, break
        if(getValue(data->tableau[indexFrom])+1 == toRank && (toRank==13||getSuit(data->tableau[indexFrom])/2 != getSuit(data->tableau[indexTo])/2)) break;
        indexFrom--;
    }

    if(-1==data->tableau[indexFrom]) {
        moveError(data, config, "%c->%c", columnFrom+'1', columnTo+'1');
    }

    //add all the cards with bigger index than indexFrom to the bottom of indexTo
    unsigned char indexIterator = indexFrom;
    while (-2!=data->tableau[indexIterator++]) setBottomColumn(data->tableau, columnTo, data->tableau[indexIterator-1]);
    data->tableau[indexFrom] = -2;
    if (indexFrom%21 > 1 && -1 == data->tableau[indexFrom-1]) {
        swapCard(data->tableau, indexFrom-1);
        data->numCovered--;
    }
}

/**
 * Reveals 1 or 3 new cards from stock (depending on rules) and adds them to top of waste
 */
void nextStockCard(struct gamestate *data, struct parserConfig *config) {
    if(!canGetFromStock(data)) moveError(data, config, ".");
    unsigned i = getBeginningOfStock(data->wasteStock), j = 0;
    if (data->turn == 1){
        swapCard(data->wasteStock, i+1);
        data->numWaste++;
        data->numStock--;
    } else if (data->turn == 3){
        while (data->wasteStock[i] != -2 && j < 3){
            if (data->wasteStock[i+1] == -2) break;
            swapCard(data->wasteStock, i+1);
            data->numWaste++;
            data->numStock--;
            j++; i++;
        }
    }
}

/**
 * If there are no cards remaining in stock and there are cards in waste
 * turn over the waste and use it as stock again. Bottom card of waste becomes
 * top card of stock, and top card of waste becomes bottom of stock.
 */
void resetStockPile(struct gamestate *data, struct parserConfig *config){
    if (!canResetStockPile(data)) moveError(data, config, "r");
    if (data->limit > 0) (data->limit)--;
    unsigned i = getBeginningOfStock(data->wasteStock);
    while (i > 0){
        swapCard(data->wasteStock, i);
        i--;
    }
    data->numStock=data->numWaste;
    data->numWaste=0;
}

/**
 * Prints the output of a failed move, in exchange format if requested.
 * We realized about 9 parameters in that a struct with all this information would come in handy. But it was too late.
 */
void moveError(struct gamestate *data, struct parserConfig *config, char *failingMessage, ...) {
    //Print fail and exit
    printf("Move %d is illegal: ", config->currentMove);

    va_list args;
    va_start(args, failingMessage);
    vfprintf(stdout, failingMessage, args); //much to Cody's dismay, Magnus decided this was important
    va_end(args);

    printf("\n");
    printConfiguration(data, config);
    exit(0);
}

/**
 * Returns 1 if the stockpile can be reset. This means the waste has cards, the stock does not, and the limit has not been overrun.
 */
char canResetStockPile(struct gamestate *data) {
    if (!(data->limit)) return 0;
    if (-1 == data->wasteStock[0]) return 0;
    unsigned i = getBeginningOfStock(data->wasteStock);
    if (data->wasteStock[i+1] != -2) return 0;
    return 1;
}

/**
 * Returns 1 if new cards can be put on the waste. This means the stock still has cards.
 */
char canGetFromStock(struct gamestate *data) {
    unsigned i = getBeginningOfStock(data->wasteStock);
    if (data->wasteStock[i+1] == -2) return 0;
    return 1;
}

/**
 * Returns 1 if a tableau column can be moved. This means that the columnTo has bottom card of suit opposite and rank 1 higher than one of the face-up cards in columnFrom.
 */
char canMoveTableauColumn(struct gamestate *data, char columnTo, char columnFrom) {
    unsigned char indexTo = getBottomColumn(data->tableau, columnTo);
    unsigned char indexFrom = getBottomColumn(data->tableau, columnFrom);

    char toRank = getValue(data->tableau[indexTo]);
    if (-1==data->tableau[indexTo]) toRank=13;

    //move indexFrom up through its column until we find -1 or we find a match
    while (data->tableau[indexFrom]!=-1) {
        //if rank is one less, and suit is opposite, break
        if(getValue(data->tableau[indexFrom])+1 == toRank && (toRank==13||getSuit(data->tableau[indexFrom])/2 != getSuit(data->tableau[indexTo])/2)) break;
        indexFrom--;
    }

    if(-1==data->tableau[indexFrom]) return 0;
//    if(indexFrom%21 == 1 && 12 == getValue(data->tableau[indexFrom])) return 0;
    return 1;
}

char winnableMoveTableauColumn(struct gamestate *data, char columnTo, char columnFrom) {
    unsigned char indexTo = getBottomColumn(data->tableau, columnTo);
    unsigned char indexFrom = getBottomColumn(data->tableau, columnFrom);

    char toRank = getValue(data->tableau[indexTo]);
    if (-1==data->tableau[indexTo]) toRank=13;

    //move indexFrom up through its column until we find -1 or we find a match
    while (data->tableau[indexFrom]!=-1) {
        //if rank is one less, and suit is opposite, break
        if(getValue(data->tableau[indexFrom])+1 == toRank && (toRank==13||getSuit(data->tableau[indexFrom])/2 != getSuit(data->tableau[indexTo])/2)) break;
        indexFrom--;
    }

    if(-1==data->tableau[indexFrom]) return 0;
    if(indexFrom%21 == 1 && 12 == getValue(data->tableau[indexFrom])) return 0;
    return 1;
}

/**
 * Returns 1 if a tableau column can be moved to the foundations. This means that there is a card on that tableau column and it has rank one greater than the foundation card of that suit.
 */
char canMoveTableauToFoundation(struct gamestate *data, char columnFrom) {
    unsigned char indexFrom = getBottomColumn(data->tableau, columnFrom);
    if (-1 == data->tableau[indexFrom]) return 0;
    char rank = getValue(data->tableau[indexFrom]);
    char suit = getSuit(data->tableau[indexFrom]);
    if (getValue(data->foundations[(unsigned) suit] != rank-1)) return 0;
    return 1;
}

/**
 * Returns 1 if the card on the waste can be moved to the tableau. This means that the card on top of the waste exists and it is a king and moving to an empty space, or it is moving below a card of opposite suit and rank one higher.
 */
char canMoveWasteToTableau(struct gamestate *data, char columnTo) {
    char topCard = getBeginningOfStock(data->wasteStock) - 1;
    unsigned char indexTo = getBottomColumn(data->tableau, columnTo);
    if (topCard == -1) return 0;

    char rankWaste = getValue(data->wasteStock[(unsigned) topCard]);
    char suitWaste = getSuit(data->wasteStock[(unsigned) topCard]);
    char rankTableau = getValue(data->tableau[(unsigned) indexTo]);
    char suitTableau = getSuit(data->tableau[(unsigned) indexTo]);

    if (rankWaste == 12 && data->tableau[(unsigned) indexTo] == -1 && data->tableau[(unsigned) indexTo+1] == -2){
        return 1;
    } else if (data->tableau[(unsigned) indexTo] != -1 && rankWaste == rankTableau-1 && suitWaste/2 != suitTableau/2){
        return 1;
    } else return 0;
}

/**
 Returns 1 if the card on the waste can be moved to the foundations. This means that the card on the waste exists and it is 1 more than what is there on the foundations
 */
char canMoveWasteToFoundations(struct gamestate *data) {
    char index = getBeginningOfStock(data->wasteStock) - 1;
    if (index == -1) return 0;
    char rank = getValue(data->wasteStock[(int) index]);
    char suit = getSuit(data->wasteStock[(int) index]);
    if (getValue(data->foundations[(unsigned) suit]) != rank-1) return 0;
    return 1;
}

/**
 * Returns 1 if the given moveID can be performed on the given gamestate
 */
char canPerformMoveID(struct gamestate *state, unsigned char moveID) {
    if (0==moveID) {fprintf(stderr, "PROGRAMMER ERROR 0 IN CANPERFORMMOVEID"); exit(1);}
    else if (58==moveID) return canGetFromStock(state);
    else if (59==moveID) return canResetStockPile(state);
    else if (1==moveID) return canMoveWasteToFoundations(state);
    else if (moveID < 9) return canMoveTableauToFoundation(state, moveID-2);
    else if (moveID < 16) return canMoveWasteToTableau(state, moveID-9);
    else if (moveID < 58) {
        char firstNum = (moveID-16)/6;
        char lastNum = (moveID-16)%6;
        if (lastNum >= firstNum) lastNum++;
        return winnableMoveTableauColumn(state, lastNum, firstNum);
    }
    fprintf(stderr, "PROGRAMMER ERROR IN CANPERFORMMOVEID");
    exit(1);
}

/**
 * Performs the given moveID on the gamestate.
 * MUST CHECK TO MAKE SURE THIS MOVE IS VALID BEFORE PERFORMING IT, AS THIS WILL CRASH OTHERWISE!!!
 */
void performMoveID(struct gamestate *state, unsigned char moveID) {
    if (0==moveID) {fprintf(stderr, "PROGRAMMER ERROR 0 IN PERFORMMOVEID"); exit(1);}
    else if (58==moveID) nextStockCard(state,0);
    else if (59==moveID) resetStockPile(state,0);
    else if (1==moveID) moveWasteToFoundation(state,0);
    else if (moveID < 9) moveTableauToFoundation(state, 0, moveID-2);
    else if (moveID < 16) moveWasteToTableau(state, 0, moveID-9);
    else if (moveID < 58) {
        char firstNum = (moveID-16)/6;
        char lastNum = (moveID-16)%6;
        if (lastNum >= firstNum) lastNum++;
        moveTableauColumn(state, 0, lastNum, firstNum);
    }
    else {
        fprintf(stderr, "PROGRAMMER ERROR IN PERFORMMOVEID");
        exit(1);
    }
}

/**
 * Prints the given moveID based on the passed in parserConfig's outfile
 */
void printExchangeMoveID(struct parserConfig *config, unsigned char moveID) {
    if (0==moveID) {fprintf(stderr, "PROGRAMMER ERROR 0 IN PRINTMOVEID"); exit(1);}
    else if (58==moveID) fprintf(config->outFile,".   ");
    else if (59==moveID) fprintf(config->outFile,"r   ");
    else if (1==moveID) fprintf(config->outFile,"w->f");
    else if (moveID < 9) fprintf(config->outFile,"%d->f",moveID-1);
    else if (moveID < 16) fprintf(config->outFile,"w->%d",moveID-8);
    else if (moveID < 58) {
        char firstNum = (moveID-16)/6;
        char lastNum = (moveID-16)%6;
	if (lastNum >= firstNum) lastNum++;
        fprintf(config->outFile,"%d->%d",firstNum+1,lastNum+1);
    }
    fprintf(config->outFile," ");
}
