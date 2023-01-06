//
// Created by Cata on 2/10/2020.
//

#ifndef COMS327PROJECT_PASER_H
#define COMS327PROJECT_PASER_H

#include "simplicity.h"
void parseCheck(struct gamestate *data, struct parserConfig *config);
void parseAdvance(struct gamestate *data, struct parserConfig *config);


void parseRules(struct gamestate *data, struct parserConfig *config);
void parseFoundations(struct gamestate *data, struct parserConfig *config, char *cards);
void parseTableau(struct gamestate *data, struct parserConfig *config, char *cards);
void parseStock(struct gamestate *data, struct parserConfig *config, char *cards);
void parseMoves(struct gamestate *data, struct parserConfig *config);

void parseFoundationPile(struct gamestate *data, struct parserConfig *config, char *cards, char suit);
void parseTableauLine(struct gamestate *data, struct parserConfig *config, char *cards, char lineNumber);
void parseStockPile(struct gamestate *data, struct parserConfig *config, char *cards);

void parseAdvancedMoves(struct gamestate *data, struct parserConfig *config);
void parseSingleMove (struct gamestate *data, struct parserConfig *config);
char parseNextIsMoveStart(FILE*);
char parseNextIsMoveEnd(FILE*);

void parseWhitespace(FILE *readFile, unsigned *line);

#endif //COMS327PROJECT_PASER_H
