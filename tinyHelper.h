//
// Created by Cata on 2/25/2020.
//

#ifndef COMS327PROJECT_TINYHELPER_H
#define COMS327PROJECT_TINYHELPER_H
void addCard(char card, char *cards);
void complain(unsigned *line, char *message,...);
char getValue(char card);
char getSuit(char card);
void fillFoundation(char card, char *cards);
char formatRankToNum(char);
char formatSuitToNum(char);
char formatNumToRank(char);
char formatNumToSuit(char);
void fprintCard(char card, FILE* outfile,char *, char *end);
unsigned getBeginningOfStock(char *wasteStock);
unsigned char getBottomColumn(char *tableau, char column);
//char getTopWaste(char *wasteStock);
char removeTopWaste(char *wasteStock);
void setBottomColumn(char *tableau, char column, char card);
void show_array(char*,int,char*);
void swapCard(char *, unsigned num);
#endif //COMS327PROJECT_TINYHELPER_H
