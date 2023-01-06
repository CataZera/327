#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "tinyHelper.h"
#include "smallHelper.h"
#include "thiccHelper.h"
#include "moves.h"
#include "simplicity.h"

//#define DEBUG_PRINT
#define SEPARATE_CARDS -1
#define END_OF_ARRAY -2

/*
* Checks the file by parsing it entirely and determines if file is valid or not
*/
void parseCheck(struct gamestate *data, struct parserConfig *config) {
    char cards[52]={0};

    data->numCovered=0;
    data->numStock  =0;
    data->numWaste  =0;

    parseRules(data, config);
    parseFoundations(data, config, cards);
    parseTableau(data, config, cards);
    parseStock(data, config, cards);
    parseMoves(data, config);

    checkCards(cards);
/*    fprintf(stdout, "Input file is valid\n");
    fprintf(stdout, "%d covered cards\n", data->covered);
    fprintf(stdout, "%d stock cards\n", data->numStock);
    fprintf(stdout, "%d waste cards\n", data->numWaste);*/
}

/*
 * Parses the file and outputs changes to foundation, tableau and the stock
 */
void parseAdvance(struct gamestate *data, struct parserConfig *config){
    /*char cards[52]={0};
    parseRules(data, config);
    parseFoundations(data, config, cards);
    parseTableau(data, config, cards);
    parseStock(data, config, cards);
    checkCards(cards);*/

    parseCheck(data, config);

    #ifdef DEBUG_PRINT
        show_array(data->wasteStock,54, "Waste and Stock");
        show_array(data->foundations,4,"Foundations");
        int i;
        for (i = 0; i<7; i++) show_array(data->tableau+i*21,21,"Tableau line");
        tableauPrinting(data->tableau, stdout);
    #endif

    parseAdvancedMoves(data, config);
}

/*
* Deletes all whitespace, ignores all comments, newlines, and tabs until character is found.
*/
void parseWhitespace(FILE *readFile, unsigned *line) {
    while (1) {
        char c = fgetc(readFile);
        switch (c) {
            case '#':
                while ('\n'!=c) {
                    c = fgetc(readFile);
                    if (EOF==c){
                        ungetc(c, readFile);
                        return;
                    }
                }
            case '\n':
            case '\r':
                (*line)++;
            case ' ':
            case '\t':
                break;
            default:
                ungetc(c, readFile);
                return;
        }
    }
}

/*
* Checks the rules to get the turn value and limit
*/
void parseRules(struct gamestate *data, struct parserConfig *config) {
    parseWhitespace(config->inFile, &(config->currentLine));

    if (!assertNextWord(config->inFile, &(config->currentLine), "RULES:")) complain(&(config->currentLine), "RULES section not found!");
    parseWhitespace(config->inFile, &(config->currentLine));

    //Parse the turn
    if (!assertNextWord(config->inFile, &(config->currentLine),"turn")) complain(&(config->currentLine), "Turn not found.");
    //A single space seperates the word turn from the number
    char space = fgetc(config->inFile);
    if (' '!=space) complain(&(config->currentLine),"Single space expected between 'turn' and the turn value.");

    data->turn = getNextInt(config->inFile, &(config->currentLine));
    if (data->turn != 1 && data->turn != 3) {
        int length = snprintf(NULL, 0, "Illegal value (%d) for turn. Should be 1 or 3. ",data->turn); //Get the length of our error message, ending in space for null terminator
        char illegalRuleException[length];
        snprintf(illegalRuleException, length, "Illegal value (%d) for turn. Should be 1 or 3.", data->turn);

        complain(&(config->currentLine), illegalRuleException);
    }
    parseWhitespace(config->inFile, &(config->currentLine));

    //Parse the limit
    if (assertNextWord(config->inFile, &(config->currentLine), "unlimited")) {
        data->limit = -1;
    } else if (assertNextWord(config->inFile, &(config->currentLine), "limit")) {
        //A single space seperates the word turn from the number
        char space = fgetc(config->inFile);
        if (' '!=space) complain(&(config->currentLine),"Single space expected between 'limit' and the limit value.");

        data->limit = getNextInt(config->inFile, &(config->currentLine));
        if (data->limit > 9 || data->limit < 0) {
            int length = snprintf(NULL, 0, "Illegal value (%d) for limit. Should be nonnegative and less than 10. ", data->limit); //Get the length of our error message, ending in space for null terminator
            char illegalRuleException[length];
            snprintf(illegalRuleException, length, "Illegal value (%d) for limit. Should be nonnegative and less than 10.", data->limit);
            complain(&(config->currentLine), illegalRuleException);
        }
    } else complain(&(config->currentLine), "Limit not found!");
    parseWhitespace(config->inFile, &(config->currentLine));

    #ifdef DEBUG_PRINT
    printf("After RULES parsing | line: %d, turn: %d, limit: %d\n", config->currentLine, data->turn, data->limit);
    #endif
}

/*
* Checks the foundations and modifies the card array to indicate
* which cards are in the foundations.
*/
void parseFoundations(struct gamestate *data, struct parserConfig *config, char *cards) {
    parseWhitespace(config->inFile, &(config->currentLine));

    if (!assertNextWord(config->inFile, &(config->currentLine), "FOUNDATIONS:")) complain (&(config->currentLine), "FOUNDATIONS section not found!");

    parseFoundationPile(data, config, cards, formatSuitToNum('c'));  //clubs
    parseFoundationPile(data, config, cards, formatSuitToNum('d'));  //diamonds
    parseFoundationPile(data, config, cards, formatSuitToNum('h'));  //hearts
    parseFoundationPile(data, config, cards, formatSuitToNum('s'));  //spades

    parseWhitespace(config->inFile, &(config->currentLine));

    #ifdef DEBUG_PRINT
    printf("After FOUNDATIONS parsing | cards: ");
    printCards(cards);
    printf("\n");
    #endif
}

/*
* Helper method for parseFoundation used to parse the foundation pile
*/
void parseFoundationPile(struct gamestate *data, struct parserConfig *config, char *cards, char suit) {
    parseWhitespace(config->inFile, &(config->currentLine));
    char suitFound = -1;
    //check for a blank pile
    char testEmpty = fgetc(config->inFile);
    if ('_'==testEmpty) {
        //we still have to check for the suit
        char testSuit = fgetc(config->inFile);
        if (EOF == testSuit) complain(&(config->currentLine), "Expected foundation pile but file ended unexpectedly.");
        suitFound = formatSuitToNum(testSuit); //Note for myself (cata) this can be reworked to get rid of redundancy.
        if (-1==suitFound) complain(&(config->currentLine), "Expected an empty foundation pile's suit but found gibberish.");
        data->foundations[(int)suit]=-1;
    } else {
        ungetc(testEmpty, config->inFile);
        char card = getNextCard(config->inFile, &(config->currentLine));
        suitFound = getSuit(card);
        //The suit could be wrong, but if it is then cards doesn't matter so we can just call this and test outside the loop
        fillFoundation(card, cards);
        data->foundations[(int)suit]=getValue(card);
    }

    //Check for the correct suit
    if (suitFound != suit) {
        char foundSuit[9];
        char expectedSuit[9];

        char suits[4][9] = {"clubs","spades","diamonds","hearts"};

        memcpy(foundSuit,suits[(unsigned) suitFound],9);
        memcpy(expectedSuit, suits[(unsigned) suit],9);

        int length = snprintf(NULL, 0, "Invalid foundation pile suit. Expected %s but was %s. The order should be clubs, diamonds, hearts, spades. ",expectedSuit, foundSuit); //Get the length of our error message, ending in space for null terminator
        char cardFormatException[length];
        snprintf(cardFormatException, length, "Invalid foundation pile suit. Expected %s but was %s. The order should be clubs, diamonds, hearts, spades.", expectedSuit, foundSuit);

        complain(&(config->currentLine), cardFormatException);
    }
}

/*
* Checks the tableau
*/
void parseTableau(struct gamestate *data, struct parserConfig *config, char *cards) {
    parseWhitespace(config->inFile, &(config->currentLine));

    if (!assertNextWord(config->inFile, &(config->currentLine), "TABLEAU:")) complain(&(config->currentLine), "TABLEAU section not found!");
    parseWhitespace(config->inFile, &(config->currentLine));

    int i;
    for (i = 0;i<7; i++)
        parseTableauLine(data, config, cards, i);

    #ifdef DEBUG_PRINT
    printf("After TABLEAU parsing | cards: ");
    printCards(cards);
    printf(", covered: %d\n", data->numCovered);
    #endif
}

/*
 * Helper method for parseTableau, parses each line in the tableau
 */
void parseTableauLine(struct gamestate *data, struct parserConfig *config, char *cards, char lineNumber) {
    parseWhitespace(config->inFile, &(config->currentLine));

    char indexInTableauLine = 0;
    //get covered cards until a | appears
    while (!assertNextWord(config->inFile, &(config->currentLine), "|")) {
        char nextCoveredCard = getNextCard(config->inFile, &(config->currentLine));

        addCard(nextCoveredCard, cards);
        data->numCovered++;
        data->tableau[lineNumber*21+(int)indexInTableauLine++] = nextCoveredCard;

        //Consume the spaces between this card and the next
        char eatSpaces = fgetc(config->inFile);
        if (eatSpaces != ' ') complain(&(config->currentLine), "At least one space must seperate tableau cards.");
        while (eatSpaces==' ') eatSpaces = fgetc(config->inFile);
        ungetc(eatSpaces, config->inFile);
    }

    data->tableau[lineNumber*21+(int)indexInTableauLine++] = SEPARATE_CARDS;

    //get uncovered cards until a \n or comment appears
    char checkNext, spaceExisted=0, noMoreCards=0;
    char lastRank = -1; char lastBlack = -1;

    while (!noMoreCards) {
        //If there is a card, there must be spaces before it
        //After the last card can be any whitespace, which may mean right after the |
        checkNext = fgetc(config->inFile);
        switch(checkNext) {
            case ' ':     spaceExisted=1; break;
            case '\n':
            case '\r':
            case '#':
            case '\t':     noMoreCards=1;
                ungetc(checkNext, config->inFile);

                //we are done, so leave
                parseWhitespace(config->inFile, &(config->currentLine));
                break;

            default:    //here we hit a card, so it better have had a space before it
                if (!spaceExisted) complain(&(config->currentLine), "At least one space must seperate tableau cards.");
                ungetc(checkNext, config->inFile);
                char nextUncoveredCard = getNextCard(config->inFile, &(config->currentLine));

                //Check to make sure this card is of opposite color and one less than the preceding card
                if (-1 == lastRank) { //First card
                    lastRank = getValue(nextUncoveredCard);
                    lastBlack = (getSuit(nextUncoveredCard)<2);
                } else {
                    if (getValue(nextUncoveredCard) == lastRank-1) {                 //If this card is one less than the last card
                        if (lastBlack != (getSuit(nextUncoveredCard) < 2)) {         //if the last card was black and this one is red, or vice versa
                            lastBlack = !lastBlack;                        //set the last card's color to this one
                            lastRank--;
                        } else {complain(&(config->currentLine), "Uncovered tableau cards must have color opposite the card they are placed upon.");}
                    } else {complain (&(config->currentLine), "Uncovered tableau cards must have rank one less than the card they are placed upon.");}
                }

                addCard(nextUncoveredCard, cards);
                data->tableau[lineNumber*21+(int)indexInTableauLine++] = nextUncoveredCard;

                spaceExisted = 0; //check for a space between this and the next card
        }
    }
    if (1 != indexInTableauLine && -1 == data->tableau[lineNumber*21 + (int)indexInTableauLine-1]) complain(&(config->currentLine), "A tableau line must not end in a covered card!");
    data->tableau[lineNumber*21+(int)indexInTableauLine++] = END_OF_ARRAY;
}

/*
* Checks which cards are in the stock and adds them to
* the card array
*/
void parseStock(struct gamestate *data, struct parserConfig *config, char *cards) {
    parseWhitespace(config->inFile, &(config->currentLine));

    if (!assertNextWord(config->inFile, &(config->currentLine), "STOCK:")) complain(&(config->currentLine), "STOCK section not found");
    parseWhitespace(config->inFile, &(config->currentLine));

    parseStockPile(data, config, cards);

    #ifdef DEBUG_PRINT
    printf("After Stock Pile parsing | ");
    printCards(cards);
    fprintf(stdout, ", stock cards: %d", data->numStock);
    fprintf(stdout, ", waste cards: %d\n", data->numWaste);
    #endif
}

/*
 * Helper method of parseStock and parses waste then stock
 */
void parseStockPile(struct gamestate *data, struct parserConfig *config, char *cards){
   parseWhitespace(config->inFile, &(config->currentLine));

    char wasteStockIndex = 0;

   //get values that are shown
    while(!assertNextWord(config->inFile, &(config->currentLine), "|")){  //repeat until you arrive at the top of the stock
        char getWasteCard = getNextCard(config->inFile, &(config->currentLine));

        addCard(getWasteCard, cards);

        data->numWaste++;
    data->wasteStock[(int)wasteStockIndex++] = getWasteCard;

       parseWhitespace(config->inFile, &(config->currentLine));
    }
    data->wasteStock[(int)wasteStockIndex++] = SEPARATE_CARDS;
    //get cards that aren't visible yet
    parseWhitespace(config->inFile, &(config->currentLine));
   while(!assertNextWord(config->inFile, &(config->currentLine), "M")){
       char getStockCard = getNextCard(config->inFile, &(config->currentLine));
       addCard(getStockCard,cards);
       data->wasteStock[(int)wasteStockIndex++] = getStockCard;
       data->numStock++;
       parseWhitespace(config->inFile, &(config->currentLine));
   }
    ungetc('M', config->inFile);
    data->wasteStock[(int)wasteStockIndex++] = END_OF_ARRAY;
}

/*
* Checks for the existence of header MOVES: in a file
*/
void parseMoves(struct gamestate *data, struct parserConfig *config) {
    parseWhitespace(config->inFile, &(config->currentLine));

    if (!assertNextWord(config->inFile, &(config->currentLine), "MOVES:")) complain(&(config->currentLine), "MOVES section not found");

    return;
}

/*
 * Parses the moves list and determines what moves to make based on valid moves in the file
 */
void parseAdvancedMoves(struct gamestate *data, struct parserConfig *config){
    //parseMoves(data, config);
    parseWhitespace(config->inFile, &(config->currentLine));
    config->currentMove = 1;
    while (!feof(config->inFile) && (config->maxMovesToRead == -1 || config->currentMove <= config->maxMovesToRead)) {
        parseSingleMove(data, config);
        (config->currentMove)++;
        parseWhitespace(config->inFile, &(config->currentLine));
    }

/*    fprintf(stdout,"Processed %u %s\n", config->currentMove-1, (config->currentMove == 2)? "move, which was valid":"moves, all valid");
    printConfiguration(data, config);*/
}

void parseSingleMove (struct gamestate *data, struct parserConfig *config) {
    char start, end;
    if (!(start=parseNextIsMoveStart(config->inFile))) complain(&(config->currentLine), "Invalid move input formatting - wrong source character for a move");
    if (start == '.') {
        nextStockCard(data, config);
        return;
    }
    if (start == 'r') {
        resetStockPile(data, config);
        return;
    }
    if (!assertNextWord(config->inFile, &(config->currentLine), "->")) complain(&(config->currentLine), "Invalid move input formatting - no arrow between source and destination");
    if (!(end=parseNextIsMoveEnd(config->inFile))) complain(&(config->currentLine), "Invalid move input formatting - wrong destination character for a move");

    if (start=='w' && end=='f') moveWasteToFoundation(data, config);
    if (start=='w' && end!='f') moveWasteToTableau(data, config, end-'1');
    if (start!='w' && end=='f') moveTableauToFoundation(data, config, start-'1');
    if (start!='w' && end!='f') moveTableauColumn(data, config, end-'1',start-'1');
    return;
}

char parseNextIsMoveStart(FILE* readFile) {
    char retVal = fgetc(readFile);
    if (retVal=='w' || retVal=='.' || retVal =='r') return retVal;
    if (retVal>'0'&&retVal<'8') return retVal;
    ungetc(retVal, readFile);
    return 0;
}

char parseNextIsMoveEnd(FILE* readFile) {
    char retVal = fgetc(readFile);
    if (retVal=='f') return retVal;
    if (retVal>'0'&&retVal<'8') return retVal;
    ungetc(retVal, readFile);
    return 0;
}
