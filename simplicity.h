//
// Created by Cata on 3/10/2020.
//
#include <stdio.h>

#ifndef COMS327PROJECT_SIMPLICITY_H
#define COMS327PROJECT_SIMPLICITY_H
//Magnus' notes: I'm going to split this up into a struct gamestate and a struct parseConfig.
//Technically it could work the way it is, but by doing so we save like 20 to 30 bytes for each of the 17 million things in the hashtable
/*struct simplicity {
    FILE *inFile;
    FILE *outFile;
    unsigned line;
    unsigned currentMove;
    int maxRead;
    char foundations[4];
    char wasteStock[54];
    char tableau[147];
    char turn;
    char limit;
    char numCovered;
    char numStock;
    char numWaste;
    char isExchange;
};*/

/**
 * Structure containing all the information relating to a single game of Klondlike
 */
struct gamestate {
    char foundations[4];
    char wasteStock[26];
    char tableau[147];

    char turn;
    char limit;

    char numCovered; //if 0 this helps with knowing if winning
    char numStock; //if Empty this helps with knowing if winning
    char numWaste; //if numWaste only contains 1 or 0 this helps know if winning
};

/**
 * Structure containing all the information pertaining to how to parse input across all check, advance, brute-force-erizor
 */
struct parserConfig {
    FILE *inFile;
    FILE *outFile;

    unsigned currentLine; //line
    unsigned currentMove;

    int maxMovesToRead; //maxRead
    unsigned maxDepthToSearch;

    char isExchange;
    char verbose;
};

/**
 * Creates (from the heap) a copy of the given gamestate.
 * This is a deep copy, though on further analysis it is not surprising considering how structs work.
 */
struct gamestate* copyGameState(struct gamestate *toCopy);

/**
 * Initializes the arguments of a parserConfig to what it should be with no arguments
 */
void initParserConfig (struct parserConfig *config);

/*not needed
void initGamestate(struct gamestate *data);*/
#endif //COMS327PROJECT_SIMPLICITY_H
