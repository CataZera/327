#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "simplicity.h"
#include "tinyHelper.h"
#include "thiccHelper.h"
#include "moves.h"

char getNextCardADD(char *cardsExist);
unsigned char randInt(unsigned char max);
//void makestate(struct gamestate *randstate, unsigned movesToDo, unsigned foundationsToLay,char printBeforeMove, char printAfterDone, unsigned rand);
char *getLegalMoveList(struct gamestate *randstate);

//int main(int argc, char **argv) {
//unsigned movesToDo = 0, foundationsToLay=0; char debugMovePrint=0, debugDonePrint=0; unsigned char i;
//	for (i = 1; i < argc; i++){
//		if (!strcmp(argv[i], "-m")) {sscanf(argv[i+1], "%u", &movesToDo); i++;}
//		else if (!strcmp(argv[i], "-f")) {sscanf(argv[i+1], "%u", &foundationsToLay); i++;}
//		else if (!strcmp(argv[i], "-v")) debugDonePrint=1;
//		else if (!strcmp(argv[i], "--print-before-moving")) debugMovePrint = 1;
//	}
//
//	struct gamestate state;
//	makestate(&state, movesToDo, foundationsToLay,debugMovePrint, debugDonePrint, time(0));
//	struct parserConfig config;
//	initParserConfig(&config);
//	config.isExchange=1;
//	printConfiguration(&state, &config);
//}

void makestate(struct gamestate *randstate, unsigned movesToDo, unsigned foundationsToLay,char printBeforeMove, char printAfterDone,
               unsigned rand) {
	if (foundationsToLay > 24) exit(0);
//	srand(time(0));
    srand(rand);

	struct parserConfig errconf;
	initParserConfig(&errconf);
	errconf.outFile = stderr;
	errconf.isExchange = 1;

	randstate->turn = (randInt(2))?1:3;
	randstate->limit = (randInt(2))?-1:randInt(10);

	char cardsExist[52] = {0};

	unsigned i;
	unsigned char j;
	//put in the specified number of foundation cards
	for (i=0; i<4; i++) randstate->foundations[i] = -1;
	for (i=0; i<foundationsToLay; i++) {
		unsigned char randSuit = randInt(4);
		if (randstate->foundations[randSuit] >= 12) continue;
		randstate->foundations[randSuit]++;
		cardsExist[randstate->foundations[randSuit]+randSuit*13] = 1;
	}

	//put in all the stock cards
	randstate->wasteStock[0]=-1;
	for (i=0; i<24-foundationsToLay; i++) randstate->wasteStock[1+i]=getNextCardADD(cardsExist);
	randstate->wasteStock[1+i]=-2;
	//put in the covered cards and a single uncovered card at the end of each tableau line
	for (i=0; i<7; i++) {
		//put in uncovered cards
		for (j=0; j<i; j++) {
			randstate->tableau[(6-i)*21 + j] = getNextCardADD(cardsExist);
		}
		randstate->tableau[(6-i)*21 + j++] = -1;
		randstate->tableau[(6-i)*21 + j++] = getNextCardADD(cardsExist);
		randstate->tableau[(6-i)*21 + j++] = -2;
	}

	//todo get num waste stock covered, currently easy but might change this later
	randstate->numWaste = getBeginningOfStock(randstate->wasteStock);
	randstate->numCovered = 0;
	for (i=0; i<7; i++) {
		randstate->numCovered += getBeginningOfStock(randstate->tableau + i*21);
	}
	randstate->numStock = getBottomColumn(randstate->wasteStock, 6) - randstate->numWaste;

	if (printBeforeMove) {
		printConfiguration(randstate, &errconf);
	}
	//while we have random moves still to do, if there are any legal moves, perform a legal move at random
	for (i=1; i<=movesToDo; i++) {
		char *legalMoveList = getLegalMoveList(randstate);
		unsigned char sizeList = strlen(legalMoveList);
		if (sizeList) {
			unsigned char todo = randInt(sizeList);
			for (j=0; j<2 && legalMoveList[todo] > 58; j++) todo = randInt(sizeList); //stock moves are boring
			performMoveID(randstate, legalMoveList[todo]);
			if (printAfterDone) {
				printExchangeMoveID(&errconf, legalMoveList[todo]);
				if (!(i%5) || i==movesToDo) fprintf(stderr,"\n");
			}
		} else {
			fprintf(stderr,"No legal moves from this point!\n");
			break;
		}
	}
	if (printAfterDone) {
		printConfiguration(randstate, &errconf);
	}
}

char *getLegalMoveList(struct gamestate *randstate) {
	static char retval[60]; //59 moves and 1 null terminator
	unsigned char i, addi=0;
	for (i=1; i<60; i++) if (canPerformMoveID(randstate, i)) retval[addi++] = i;
	retval[addi]=0;
	return retval;
}

/**
 * Returns the next card to add, MUST BE CALLED WITH AT LEAST ONE CARD EXISTING
 */
char getNextCardADD(char *cardsExist) {
	unsigned char tryIndex = randInt(52);
	while (cardsExist[tryIndex]) {
		tryIndex++;
		if (tryIndex > 51) tryIndex = 0;
	}
	cardsExist[tryIndex]++;
	return (char)tryIndex;
}

unsigned char randInt(unsigned char max) {
	unsigned retval = rand();
	return retval % max;
}
