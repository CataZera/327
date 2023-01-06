//
// Created by Cata on 4/6/2020.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "parser.h"
#include "simplicity.h"
#include "thiccHelper.h"
#include "tinyHelper.h"
#include "moves.h"
#include "dataStructures/linkedQueue.h"
#include "dataStructures/charStack.h"
#include "dataStructures/bfHashtable.h"

char* getSafeFoundationMoves(struct gamestate *state);
char isSafeFoundationMove(char moving, char *foundations);
void verboseSeconds( struct hashtableNode*, struct hashtable*, struct linkedQueue*, unsigned long counter, unsigned long seconds, struct parserConfig *);
void verboseMillion(struct hashtableNode *hashNode, struct hashtable *table, struct linkedQueue *queue, unsigned long counter,unsigned long seconds, struct parserConfig *);
void verbosePrint(struct hashtableNode *hashNode, struct hashtable *table, struct linkedQueue *queue,unsigned long counter, unsigned long seconds, struct parserConfig *);
void crash(char *format, ...);
char forceMoveToFoundation(struct gamestate* current);
//void updateQueueAndGameState(struct gamestate* current, struct hashtable* bfTable, unsigned i, struct linkedQueue queue, struct hashtableNode* winningPointer);

int main(int argc, char** argv){
    struct parserConfig config;
    initParserConfig(&config);
    config.outFile = stderr; //file that all program output is written to, except the small amount that is stdout

    unsigned long i, f = 0, counter=0;
    char shouldParseCheck=0;
    for (i = 1; i < argc; i++){
        if (!strcmp(argv[i], "-m")) {sscanf(argv[i+1], "%u", &(config.maxDepthToSearch)); i++;}
        else if (!strcmp(argv[i], "-c")); //Todo if we ever want -c to do something
        else if (!strcmp(argv[i], "-f")) f = 1;
        else if (!strcmp(argv[i], "-v")) config.verbose = 1;
        else if (!strcmp(argv[i], "-vv")) config.verbose = 2;
        else if (!strcmp(argv[i], "--stdout")) config.outFile = stdout;
        else if (!strcmp(argv[i], "-nomoves")) shouldParseCheck = 1;
        else config.inFile = fopen(argv[i],"r");
    }

    if (config.inFile == NULL){
        fprintf(stderr, "Error reading from the input file. It probably does not exist\n");
        return 1;
    }

    struct gamestate* data = malloc(sizeof(struct gamestate));
    if (!data) crash("Malloc fail for %d bytes! Dude upgade your pc...",sizeof(struct gamestate));
    if (shouldParseCheck) parseCheck(data, &config); else parseAdvance(data, &config);
    if (config.inFile != stdin) fclose(config.inFile); //done with the inFile
    config.inFile = stdin;

    struct hashtableNode* winningPointer = 0;
    struct hashtableNode* lastPointer = 0;

    struct hashtable bfTable;
    init_table(&bfTable, 2000000);
    struct linkedQueue queue;
    initLinkedQueue(&queue);
    struct hashtableNode* newPointer = add_to_table(&bfTable, data, 0, 0);
    enQueue(&queue, newPointer);

    if (newPointer->state->numCovered == 0 && newPointer->state->numStock == 0 && newPointer->state->numWaste <= 1) winningPointer=newPointer;

    unsigned long seconds = 1;
    time_t firstTime = time(0);

    while (!winningPointer && !isEmptyQueue(&queue)){
        struct hashtableNode* current = deQueue(&queue);
        char* safeMoves = getSafeFoundationMoves(current->state);
        lastPointer = current;
        if (f == 1 && safeMoves[0] != 0){
            if (safeMoves != NULL){
                for (i = 0; safeMoves[i] != 0; i++) {
                    if (canPerformMoveID(current->state, safeMoves[i])){
                        struct gamestate *copy = copyGameState(current->state);
                        performMoveID(copy, safeMoves[i]);
                        struct hashtableNode* tempPointer = add_to_table(&bfTable, copy, current, safeMoves[i]);
                        if (!tempPointer) {free(copy); continue;} //WE FOUND A DUPLICATE GAME STATE
                        if (copy->numCovered == 0 && copy->numStock == 0 && copy->numWaste <= 1){
                            winningPointer = tempPointer;
                            break;
                        }
                        if ((tempPointer->order) < config.maxDepthToSearch){
                            enQueue(&queue, tempPointer);
                        }
                    }
                }
                if (winningPointer != 0) { //Not really needed but a mental safety check to ease my nerves and know that this gets out of the loop
                    if (winningPointer->state->numStock == 0 && winningPointer->state->numCovered == 0 &&
                        winningPointer->state->numWaste <= 1)
                        break;
                }
            }
        } else {
            for (i = 1; i < 60; i++) {
                if (canPerformMoveID(current->state, i)) {
                    struct gamestate *copy = copyGameState(current->state);
                    performMoveID(copy, i);
                    struct hashtableNode *tempPointer = add_to_table(&bfTable, copy, current, i);
                    if (!tempPointer) {
                        free(copy);
                        continue;
                    } //WE FOUND A DUPLICATE GAME STATE
                    if (copy->numCovered == 0 && copy->numStock == 0 && copy->numWaste <= 1) {
                        winningPointer = tempPointer;
                        break;
                    }
                    if ((tempPointer->order) < config.maxDepthToSearch) {
                        enQueue(&queue, tempPointer);
                    }
                }
            }
        }
        counter++;
        if (config.verbose) verboseSeconds(current, &bfTable, &queue, counter, seconds, &config);
        //if (config.verbose) verboseMillion(current, &bfTable, &queue, counter, seconds, &config);

        if (time(0) >= firstTime+1) {
            firstTime += 1;
            seconds++;
        }
    }
    if (!winningPointer) {
        if (config.verbose > 0) {
            fprintf(config.outFile, "Final report:\n");
            fprintf(config.outFile,"Seconds elapsed: %lu\nHashtable size: %lu  |  Duplicates ignored: %lu  |  Queue Elements: %lu  |  Total states searched: %lu\n",seconds,bfTable.numElements,bfTable.numDuplicates, itemsInQueue(&queue), counter);
            unsigned char everWinnable = 0;
            if (lastPointer->order == config.maxDepthToSearch - 1 || !config.maxDepthToSearch) everWinnable = 1;
            if (everWinnable) fprintf(config.outFile,"Specifically, it is possibly winnable but you need to search further.\n\n");
            else fprintf(config.outFile, "Specifically, it is never winnable no matter how hard you try. The deepest possible move is %lu from the parsed configuration.\n\n",lastPointer->order);
        }
        fprintf (stdout, "# Game is not winnable within %d moves\n", config.maxDepthToSearch);
    } else {
        struct charStack stack;
        initStack(&stack);
        struct hashtableNode* tempPointer = winningPointer;
        while (tempPointer->moveID) {
            charPush(&stack, tempPointer->moveID);
            tempPointer = tempPointer->from;
        }
        if (config.verbose > 0) {
            fprintf(config.outFile, "Final report:\n");
            fprintf(config.outFile,"Seconds elapsed: %lu\nHashtable size: %lu  |  Duplicates ignored: %lu  |  Queue Elements: %lu  |  Total states searched: %lu\n",seconds,bfTable.numElements,bfTable.numDuplicates, itemsInQueue(&queue), counter);
            if (config.verbose > 1) {
                fprintf(config.outFile, "Winning gamestate:\n");
                printConfiguration(winningPointer->state,&config);
                fprintf(config.outFile, "\n");
            } else fprintf(config.outFile, "Use -vv to show the winning game state\n\n");
        }
        fprintf(stdout, "# Game is winnable in %ld moves\n", winningPointer->order);
        for (i=1; !charStackEmpty(&stack); i++) {
            struct parserConfig stdoutConfig;
            initParserConfig(&stdoutConfig);
            printExchangeMoveID(&stdoutConfig, charPop(&stack));
            if (!(i%5)) fprintf(stdout, "\n");
        }
        fprintf(stdout, "\n");
        destroyStack(&stack);
    }
    //free memory
    freeHashTable(&bfTable);
    destroyLinkedQueue(&queue);
}

/**
 * Returns a null-terminated array of moveIDs corresponding to the valid forced move IDs possible on the current gamestate
 * Which means an empty array (retval[0]=0) if there are none to do
 */
char* getSafeFoundationMoves(struct gamestate *state) {
    static char retval[5];
    char i;
    unsigned char index = 0;
    //test waste card
    if (1 == state->turn) {
        char index2 = getBeginningOfStock(state->wasteStock) - 1;  //get the top Waste Card
        if (-1!=index2)
            if (isSafeFoundationMove(state->wasteStock[(unsigned) index2], state->foundations))
                retval[index++] = 1;
    }
    //test tableau cards
    for (i=0; i<7; i++) {
        //get the moving tableau card
        unsigned char indexFrom = getBottomColumn(state->tableau, i);
        if (-1 == state->tableau[indexFrom]) continue;
        if (isSafeFoundationMove(state->tableau[(unsigned) indexFrom],state->foundations)) retval[index++] = 2+i;
    }
    retval[index]=0;
    return retval;
}

/**
 * Tests most of the conditions for moving card moving to the foundations, as well as most of legality
 */
char isSafeFoundationMove(char moving, char *foundations) {
    char movingSuite = getSuit(moving); //moving suit is value of the suit being moved from either waste or tableau
    if (getValue(moving)!=getValue(foundations[(unsigned) movingSuite])+1) return 0; //not a valid move
    if (getValue(foundations[!(movingSuite/2) *2])   < getValue(foundations[(unsigned) movingSuite])) return 0; //one of the opposite color foundations is less than the foundation value we are moving to
    if (getValue(foundations[!(movingSuite/2) *2+1]) < getValue(foundations[(unsigned) movingSuite])) return 0; //one of the opposite color foundations is less than the foundation value we are moving to
    return 1;
}

/** CLOCK
 * Prints every few seconds for verbose switch
 */
void verboseSeconds(struct hashtableNode *hashNode, struct hashtable *table, struct linkedQueue *queue, unsigned long counter, unsigned long seconds, struct parserConfig *config) {
    static unsigned long lastRan = 2;
    if (seconds > lastRan + 2) {
        lastRan = seconds;
        verbosePrint(hashNode,table, queue, counter, seconds, config);
    }
}

/** CLOCK
 * Prints every 250,000 gamestates it checks for verbose switch
 */
void verboseMillion(struct hashtableNode *hashNode, struct hashtable *table, struct linkedQueue *queue,unsigned long counter, unsigned long seconds, struct parserConfig *config) {
    static unsigned long lastTime = 250000;
    if (counter>=lastTime) {
        verbosePrint(hashNode,table, queue, counter, seconds, config);
        lastTime+=250000;
    }
}

/**
 * Prints a bunch of information to implement the verbose switch
 */
void verbosePrint(struct hashtableNode *hashNode, struct hashtable *table, struct linkedQueue *queue,unsigned long counter, unsigned long seconds, struct parserConfig *config) {
    fprintf(config->outFile,"Seconds elapsed: %lu\nHashtable size: %lu  |  Duplicates ignored: %lu  |  Queue Elements: %lu  |  Total states searched: %lu\nAll moves searched to depth %lu\n\n",seconds,table->numElements,table->numDuplicates, itemsInQueue(queue), counter, hashNode->order -1);
    //printSlotsOccupiedHashTable(table);fflush(stdout);fflush(stderr);
}

void crash (char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

char forceMoveToFoundation(struct gamestate* state){
    if (canPerformMoveID(state, 1)){
        if (state->turn == 1){
            char wasteRank = formatNumToRank(getValue(getBeginningOfStock(state->wasteStock) - 1));
            char wasteSuit = formatNumToSuit(getSuit(getBeginningOfStock(state->wasteStock) - 1));
            char foundationsRank1 = 0, foundationsRank2 = 0;
            if (wasteSuit == 'c' || wasteSuit == 's'){
                foundationsRank1 = formatNumToRank(state->foundations[2]);
                foundationsRank2 = formatNumToRank(state->foundations[3]);
            } else {
                foundationsRank1 = formatNumToRank(state->foundations[0]);
                foundationsRank2 = formatNumToRank(state->foundations[1]);
            }
            if(wasteRank <= foundationsRank1 + 1 && wasteRank == foundationsRank2 + 1) {
                return 1;
            }
        }
    }
    return 0;
}
