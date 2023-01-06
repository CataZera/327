//
// Created by Cata on 2/16/2020.
//

/*
 * This header file seeks to implement all the moves needed to be made in the file.
 * Each move is defined for what step is needed to be executed when move in file is parsed
 */

#ifndef COMS327PROJECT_MOVES_H
#define COMS327PROJECT_MOVES_H

#include "simplicity.h"

void moveWasteToFoundation(struct gamestate *data, struct parserConfig *config);
void moveWasteToTableau(struct gamestate *data, struct parserConfig *config, char columnTo);
void moveTableauToFoundation(struct gamestate *data, struct parserConfig *config, char columnFrom);
void moveTableauColumn(struct gamestate *data, struct parserConfig *config, char columnTo, char columnFrom);
void nextStockCard(struct gamestate *data, struct parserConfig *config);
void resetStockPile(struct gamestate *data, struct parserConfig *config);
void moveError(struct gamestate *data, struct parserConfig *config, char *failingMessage, ...);
void wasteStockEmpty(struct gamestate *data, struct parserConfig *config, char* failingMove);

char canMoveWasteToFoundations(struct gamestate *data);
char canMoveWasteToTableau(struct gamestate *data, char columnTo);
char canMoveTableauToFoundation(struct gamestate *data, char columnFrom);
char canMoveTableauColumn(struct gamestate *data, char columnTo, char columnFrom);
char canGetFromStock(struct gamestate *data);
char canResetStockPile(struct gamestate *data);

char canPerformMoveID(struct gamestate *state, unsigned char moveID);
void performMoveID(struct gamestate *state, unsigned char moveID);
void printExchangeMoveID(struct parserConfig *state, unsigned char moveID);
#endif //COMS327PROJECT_MOVES_H
