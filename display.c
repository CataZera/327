//
// Created by Cata on 4/21/2020.
//

#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "simplicity.h"
#include "thiccHelper.h"
#include "tinyHelper.h"
#include "smallHelper.h"
#include "moves.h"
#include "randState.h"

struct params{
    char source;
    int movenumber;
    struct gamestate *laststate;
    struct stackOfGameStates *stack;
    struct gamestate *currentState;
};

unsigned randIntSeed(unsigned long max){
    return rand()%max;
}

void printBlockColor(int iy, int ix, char *c){
    mvprintw(iy, ix, c);
//    move(iy, ix);
//    printw(c);
//    getch();
}

void printWords(int iy, int ix, char *c){
    mvprintw(iy, ix, c);
//    move(iy, ix);
//    printw(c);
//    getch();
}

void printNumber(int iy, int ix, char rank){
    mvprintw(iy, ix, "%c", rank);
//    move(iy, ix);
//    printw("%c", rank);
//    getch();
}

void printSuit(int iy, int ix, char c){
    if (c == 'c'){
        uint32_t club = L'\u2663';
        mvprintw(iy, ix, "%lc", club);
    } else if (c == 's'){
        uint32_t spade = L'\u2660';
        mvprintw(iy, ix, "%lc", spade);
    }else if (c == 'd'){
        uint32_t diamond = L'\u2666';
        mvprintw(iy, ix, "%lc", diamond);
    }else if (c == 'h'){
        uint32_t heart = L'\u2665';
        mvprintw(iy, ix, "%lc", heart);
//        if (c == 'c'){
//            mvprintw(iy, ix, "%c", 'c');
//        } else if (c == 's'){
//            mvprintw(iy, ix, "%c", 's');
//        }else if (c == 'd'){
//            mvprintw(iy, ix, "%c", 'd');
//        }else if (c == 'h'){
//            mvprintw(iy, ix, "%c", 'h');
//        move(iy, ix);
//        printw();
//        getch();
    }
}

void drawInstructions(char *file){
#define TITLES 5
    init_pair(TITLES, COLOR_BLACK, COLOR_GREEN);
#define BORDER_COLOR 6
    init_pair(BORDER_COLOR, COLOR_WHITE, COLOR_GREEN);
#define HIGHLIGHT_COLOR 7
    init_pair(HIGHLIGHT_COLOR, COLOR_BLACK, COLOR_MAGENTA);

    attron(COLOR_PAIR(BORDER_COLOR));
    int length = 0;
    char *c = "";
    while(file[length]) length++;
    unsigned char ix, iy;
    int printFirstCharHere = (34-(6+length))/2;
    int printHeader = (34-6)/2;
    int printSecondOption = (34-length)/2;
    for (ix = 0; ix <=33; ix++){
        for (iy = 0; iy<=9; iy++){
            if (iy == 0 || iy == 9) {
                printBlockColor(iy, ix, " ");
                //print File/Seed info on top border
                attron(COLOR_PAIR(TITLES));
                if (6+length <= 34) {
                    mvprintw(0, printFirstCharHere, "File: %s", file);
                } else{
                    mvprintw(0, printHeader, "File: ");
                    mvprintw(9, printSecondOption, "%s", file);
                }
                attroff(COLOR_PAIR(TITLES));
                //restore the border color
                attron(COLOR_PAIR(BORDER_COLOR));
            } else if ((iy == 1 && ix == 3) || (iy == 1 && ix == 16)) {
                attron(COLOR_PAIR(HIGHLIGHT_COLOR));
                c = "Q"; if (ix == 16) c = "S";
                printBlockColor(iy, ix, c);
                attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
                attron(COLOR_PAIR(BORDER_COLOR));
            } else if (iy == 2 && ix == 3){
                attron(COLOR_PAIR(HIGHLIGHT_COLOR));
                printBlockColor(iy, ix, "R");
                attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
                attron(COLOR_PAIR(BORDER_COLOR));
            } else if((iy == 3 && ix == 3) || (iy == 3 && ix == 16)){
                attron(COLOR_PAIR(HIGHLIGHT_COLOR));
                c = "N"; if (ix == 16) c = "U";
                printBlockColor(iy, ix, c);
                attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
                attron(COLOR_PAIR(BORDER_COLOR));
            } else if ((iy == 5 && ix == 3) || (iy == 5 && ix == 16)){
                attron(COLOR_PAIR(HIGHLIGHT_COLOR));
                c = "e"; if (ix == 16) c = "r";
                printBlockColor(iy, ix, c);
                attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
                attron(COLOR_PAIR(BORDER_COLOR));
            }
            else {
                if (ix == 0 || ix == 33){
                    printBlockColor(iy,ix, " ");
                }
            }
        }
    }
    attroff(COLOR_PAIR(BORDER_COLOR));
    attron(COLOR_PAIR(HIGHLIGHT_COLOR));
    printBlockColor(11,12, "F");
    attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
    attron(COLOR_PAIR(HIGHLIGHT_COLOR));
    printBlockColor(18, 5, "W");
    attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
    unsigned num = 1, i;
    char arr[10];
    attron(COLOR_PAIR(HIGHLIGHT_COLOR));
    for (i = 39; i < 79; i+=6){
        sprintf(arr, "%d", num);
        printBlockColor(0, i, arr);
        num++;
    }
    attroff(COLOR_PAIR(HIGHLIGHT_COLOR));
}

void informationText(struct gamestate *data){
    //    print Stuff inside the box
#define TEXT_COLOR 8
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(TEXT_COLOR));
    printWords(1,4, ": quit");
    printWords(1, 17 , ": save to file");
    printWords(2,4, ": restart");
    printWords(3,4, ": new game");
    printWords(3,17, ": undo move");
    printWords(5,4,": next card");
    printWords(5,17, ": reset stock");
    printWords(6,3,"To move, press the source and");
    printWords(7,3, "then destination (one of W, F,");
    printWords(8,3, "1, 2, 3, 4, 5, 6, 7)");
    printWords(11, 13, "oundations");
    printWords(18, 6, "aste");
    if (data->limit == -1) printWords(18, 11, ("(Unlimited resets)"));
    attroff(COLOR_PAIR(TEXT_COLOR));
}

void drawBlankFoundationPile(char c, unsigned ix) {
#define EMPTY_PILE_CLUB_SPADES 10
    init_pair(EMPTY_PILE_CLUB_SPADES, COLOR_BLACK, COLOR_GREEN);
#define EMPTY_PILE_H_D 11
    init_pair(EMPTY_PILE_H_D, COLOR_RED, COLOR_GREEN);
    if (c == 'c' || c == 's'){
        attron(COLOR_PAIR(EMPTY_PILE_CLUB_SPADES));
    } else{
        attron(COLOR_PAIR(EMPTY_PILE_H_D));
    }
    unsigned i, iy;
    for(i = ix; i < ix+4; i++){
        for(iy = 13; iy <= 16; iy++){
            printBlockColor(iy, i, " ");
        }
    }

    for (i = ix; i < ix+4; i++){
        for (iy = 13; iy <= 16; iy++){
            printSuit(iy, i, c);
        }
    }
    if (c == 'c' || c == 's'){
        attroff(COLOR_PAIR(EMPTY_PILE_CLUB_SPADES));
    } else{
        attroff(COLOR_PAIR(EMPTY_PILE_H_D));
    }
}

void drawCard(char card, char c, int iy, int ix){
#define CARD_BLACK 1
    init_pair(CARD_BLACK, COLOR_BLACK, COLOR_WHITE);
#define CARD_RED 2
    init_pair(CARD_RED, COLOR_RED, COLOR_WHITE);
#define CARD_BACK 3
    init_pair(CARD_BACK, COLOR_BLACK, COLOR_RED);

    if (c == 'c' || c == 's'){
        attron(COLOR_PAIR(CARD_BLACK));
    } else if (c == 'h' || c == 'd'){
        attron(COLOR_PAIR(CARD_RED));
    } else attron(COLOR_PAIR(CARD_BACK));
    unsigned x, y;
    for(x = ix; x < ix+4; x++){
        for(y = iy; y <= iy+3; y++){
            printBlockColor(y, x, " ");
        }
    }
    if (c != '#') {
        char rank = formatNumToRank(getValue(card));
        char suit = formatNumToSuit(getSuit(card));
        for (x = ix; x <= ix + 3; x++) {
            for (y = iy; y <= iy + 3; y++) {
                if ((x == ix && y == iy) || (x == ix + 3 && y == iy + 3)) {
                    printNumber(y, x, rank);
                }
                if ((x == ix + 1 && y == iy) || (x == ix + 2 && y == iy + 3)) {
                    printSuit(y, x, suit);
                }
            }
        }
    } else {
        for (x = ix; x <= ix + 3; x++) {
            for (y = iy; y <= iy + 3; y++) {
                printWords(y, x, "#");
                }
            }
        }

    if (c == 'c' || c == 's'){
        attroff(COLOR_PAIR(CARD_BLACK));
    } else if (c == 'h' || c == 'd'){
        attron(COLOR_PAIR(CARD_RED));
    } else attron(COLOR_PAIR(CARD_BACK));

}

void drawFoundations(struct gamestate *data){
#define CARD_BLACK 1
    init_pair(CARD_BLACK, COLOR_BLACK, COLOR_WHITE);
#define CARD_RED 2
    init_pair(CARD_RED, COLOR_RED, COLOR_WHITE);
#define CARD_BACK 3
    init_pair(CARD_BACK, COLOR_BLACK, COLOR_RED);

    if (data->foundations[(unsigned) formatSuitToNum('c')] == -1) drawBlankFoundationPile('c', 5);
    else drawCard(data->foundations[(unsigned) formatSuitToNum('c')], 'c', 13, 5);
    if (data->foundations[(unsigned) formatSuitToNum('d')] == -1) drawBlankFoundationPile('d', 11);
    else drawCard(data->foundations[(unsigned) formatSuitToNum('d')]+26, 'd', 13, 11);
    if (data->foundations[(unsigned) formatSuitToNum('h')] == -1) drawBlankFoundationPile('h', 17);
    else drawCard(data->foundations[(unsigned) formatSuitToNum('h')]+39, 'h', 13, 17);
    if (data->foundations[(unsigned) formatSuitToNum('s')] == -1) drawBlankFoundationPile('s', 23);
    else drawCard(data->foundations[(unsigned) formatSuitToNum('s')]+13, 's', 13, 23);
}

void drawWasteStock(struct gamestate *data){
#define TEXT_COLOR 8
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
    if (data->limit > -1) {
        mvprintw(18, 11, "(%d resets remaining)", data->limit);
    }
    drawCard('#','#', 20, 23);
    char cardTop = 0;
    char cardMiddle = 0;
    char cardBottom = 0;
    unsigned topOfWaste = -2, stackSize = 0, i = 0;
    topOfWaste = getBeginningOfStock(data->wasteStock)-1; //if -1 no card in waste
    if (topOfWaste != -1) {
        cardTop = data->wasteStock[topOfWaste];
        if (topOfWaste == 0) drawCard(cardTop, formatNumToSuit(getSuit(cardTop)), 20, 12);
        else if (topOfWaste == 1) {
            cardBottom = data->wasteStock[topOfWaste - 1];
            drawCard(cardBottom, formatNumToSuit(getSuit(cardBottom)), 20,10);
            drawCard(cardTop, formatNumToSuit(getSuit(cardTop)), 20,12);
        } else if (topOfWaste >= 2){
            cardBottom = data->wasteStock[topOfWaste - 2];
            cardMiddle = data->wasteStock[topOfWaste - 1];
            drawCard(cardBottom, formatNumToSuit(getSuit(cardBottom)), 20,10);
            drawCard(cardMiddle, formatNumToSuit(getSuit(cardMiddle)), 20,12);
            drawCard(cardTop, formatNumToSuit(getSuit(cardTop)), 20,14);
        }
    }
    i = topOfWaste+1;
    while (data->wasteStock[i] != -2) {i++; stackSize++;}
    attron(COLOR_PAIR(TEXT_COLOR));
    mvprintw(24, 12, "%d", topOfWaste+1);
    mvprintw(24, 24, "%d", stackSize - 1);
    attroff(COLOR_PAIR(TEXT_COLOR));
}

void drawTableau(struct gamestate *data){
    //getBottomofColumn
    //do each column at a time
    //print top of column down
    //count till find -1 then print those hidden cards
    char position[7] = {0};
    char i, j = 0, done = 0, space = 0, y = 1;
    while (!done){
        done = 1;
        //draw single row of tableau
        for (i = 6; i >= 0; i--){
            if (2 == position[(unsigned char) i]) continue;
            char card = data->tableau[i * 21 + position[(unsigned char)i] + j];
            if (card == -1){
                position[(unsigned char) i] = 1;
                card = data->tableau[i*21 + position[(unsigned char)i]+ j];
            }
            if (card == -2){
                position[(unsigned char) i] = 2;
            }
            if (position[(unsigned char)i] != 2){
                done = 0;
            }
        }
        if (!done){
            for (i = 6; i >= 0; i--){
                char card;
                card = data->tableau[i*21+ position[(unsigned char)i]+j];
                if (position[(unsigned char)i] == 0){
                    drawCard('#', '#', y, 38+space);
                } else if (position[(unsigned char)i] == 1){
                    drawCard(card, formatNumToSuit(getSuit(card)), y, 38+space);
                }
                space+=6;
            }
        }
        j++;
        y++;
        space = 0;
    }
}

void refreshScreen(struct gamestate *data, char *file){
    clear();
    drawInstructions(file);
    informationText(data);
    drawFoundations(data);
    drawTableau(data);
    drawWasteStock(data);
    refresh();
}

void handleKeyboardInput(char c, char source, struct gamestate *data){
    if ((source >= '1' && source <= '7') && (c != source && (c >= '1' && c <= '7'))){
        if (canMoveTableauColumn(data, c - '1', source - '1')) {
            moveTableauColumn(data, 0, c - '1', source -'1');
        }
    } else if (((source >= '1' && source <='7') && c == 'f') || (source =='f' && (c >= '1' && c <='7'))){
        if (canMoveTableauToFoundation(data, source - '1')){
            moveTableauToFoundation(data, 0, source - '1');
        }
    } else if ((source == 'w' && c == 'f') || (source == 'f' && c== 'w')) {
        if (canMoveWasteToFoundations(data)){
            moveWasteToFoundation(data, 0);
        }
    } else if (((source >= '1' && source <= '7') && c == 'w') || (source == 'w' && (c >= '1' && c <= '7'))) {
        if (source != 'w' && canMoveWasteToTableau(data, source-'1')) {moveWasteToTableau(data, 0, source-'1');
        } else if (c != 'w' && canMoveWasteToTableau(data, c -'1')) moveWasteToTableau(data, 0 , c -'1');
    }
}

void resetGame(struct gamestate *data, struct gamestate *reset){
    unsigned i;
    for (i = 0; i < 147; i++){
        if (i < 4) data->foundations[i] = reset->foundations[i];
        if (i < 26) data->wasteStock[i] = reset->wasteStock[i];
        data->tableau[i] = reset->tableau[i];
    }
    data->limit = reset->limit;
    data->turn = reset->turn;
    data->numCovered = reset->numCovered;
    data->numWaste = reset->numWaste;
    data->numStock = reset->numStock;
}

int main(int argc, char **argv) {
#define TEXT_COLOR 8
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
    struct gamestate data;
    struct parserConfig config;
    initParserConfig(&config);

    char *fileName = 0;
    unsigned i;
    for (i = 1; i < argc; i++){
        if (!strcmp(argv[i], "-f")) {
            i++;
            fileName = argv[i];
            config.inFile = fopen(argv[i], "r");
            if(config.inFile != NULL) parseCheck(&data, &config);
            else {fprintf(stderr, "Error reading from the input file. It probably does not exist\n"); return 1;}
        }
        else if (!strcmp(argv[i], "-s")) {
            unsigned num;
            sscanf(argv[i+1], "%u", &num);
            fileName  = argv[i+1];
            makestate(&data, 0, 0, 0, 0, randIntSeed(num));
            i++;
        }
        else {
            fileName = argv[i];
            config.inFile = fopen(argv[i], "r");
            if(config.inFile != NULL) parseCheck(&data, &config);
            else {fprintf(stderr, "Error reading from the input file. It probably does not exist\n"); return 1;}
//            config.outFile = fopen("tempBackUP.txt", "w");
//            c = fgetc(config.inFile);
//            while(c != EOF){
//                fputc(c, "tempBackUp.txt");
//                c = fgetc(config.inFile);
//            }
        }
    }

    setlocale(LC_ALL,"");
    initscr();
    noecho();
    curs_set(0);
    wtimeout(stdscr,1);
    keypad(stdscr,1);
    start_color();

    drawInstructions(fileName);
    informationText(&data);
    drawFoundations(&data);
    drawWasteStock(&data);
    drawTableau(&data);

    struct gamestate *undo;
    undo = copyGameState(&data);

    struct gamestate *reset;
    reset = copyGameState(&data);

    int randomAmount = 2147483647;
    char buf[20];
    int source = 0;
    while (1){
        int c = getch();
        if (c == ERR);
        else if ('Q' == c){endwin(); fclose(config.inFile); free(undo); free(reset); return 0;}
        else if ('R' == c) {resetGame(&data, reset); refreshScreen(&data, fileName);}
        else if ('U' == c) {resetGame(&data, undo); refreshScreen(&data, fileName);}
        else if ('N' == c) {
            makestate(&data, 0,0,0,0,randIntSeed(randomAmount--));
            snprintf(buf, sizeof(buf), "%d", randomAmount);
            refreshScreen(&data, buf);
        }
        else if ('S' == c) {
            config.outFile = fopen(fileName, "w");
            config.isExchange = 1;
            printConfiguration(&data, &config); fclose(config.outFile);}
        else if (c == 'e') {if (canGetFromStock(&data)) {nextStockCard(&data, 0); refreshScreen(&data, fileName); source = 0;}}
        else if (c == 'r') {if (canResetStockPile(&data)) {resetStockPile(&data, 0); refreshScreen(&data, fileName); source = 0;}}
        else if (c == 'w' && source == 0) source = 'w';
        else if (c == 'f' && source == 0) source = 'f';
        else if ((c >= '1' && c <= '7') && source == 0) source = c;
        else if (source > 0 && ((c >= '1' && c <= '7') || (c == 'w' || c == 'f'))){
            resetGame(undo, &data);
            handleKeyboardInput(c, source, &data);
            refreshScreen(&data, fileName);
            source = 0;
            c = 0;
        }
//        else {source = 0; c = 0;}
//        getch();
    }
    free(undo);
    free(reset);
    endwin();
    return 0;
}




//    setlocale(LC_ALL,"");  //this gives unit code character.  Have to call this before initscr()!!!
//good strat is to check the file first by parsing it and then do initscr();
//    initscr();
//    noecho(); //stops the display from updating changes with input
//    wtimeout(stdscr, 1); // if 2nd argument is 0, dont wait for input. if 1, wait. if 2, wait for enter
//    curs_set(0);  //stops showing the cursor position
//    keypad(stdscr, 1);    //this makes it easy to read input
//    mousemask(ALL) read into this
//    start_color();
//#define Card_front 1
//    init_pair(Card_front, COLOR_RED, COLOR_MAGENTA);

//    printw("Hello world? %lc  \n", L'\u2713');   //lc = long character
//    attron(COLOR_PAIR(Card_front));  //changes whatever printing to that color
//attron used every time you print
//    printw("COLORFUL");
//    mvprintw()
//    attron(COLOR_PAIR(0)); //sets it back to normal
//    int i = getch() - '0';
//    while (i--) {
//        int c = getch(); //if wtimeout is 0, then return ERR if no input
//      if(c == ERR) ERR is a char that can be checked
//        printw("\n");
//        getch();
//    };
//    getch();
//    endwin(); //before you exit/return from any method you need to call this! And only after you call initscr();
//    return 0;