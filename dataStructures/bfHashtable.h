//
// Created by Cata on 4/2/2020.
//
#ifndef COMS327PROJECT_BFHASHTABLE_H
#define COMS327PROJECT_BFHASHTABLE_H
#include "../simplicity.h"
struct hashtableNode {
    struct hashtableNode* nextInList; //iterator for next in the list of hashtable
    struct gamestate *state;
    struct hashtableNode* from; //pointer from the previous gamestate
    char moveID;
    unsigned long order;
};

struct hashtable{
    struct hashtableNode** hash;
    unsigned long size;
    unsigned long numElements;
    unsigned long numDuplicates;
};

void init_table(struct hashtable* bf, unsigned long size);
unsigned long hash(struct gamestate* data);
struct hashtableNode* add_to_table(struct hashtable* bf, struct gamestate* data, struct hashtableNode* from, char moveID);
char stateEquals(struct gamestate* state1, struct gamestate* state2);
void freeHashTable(struct hashtable* bf);
void printSlotsOccupiedHashTable(struct hashtable* bf);
#endif //COMS327PROJECT_BFHASHTABLE_H

/*

hashtable: O 1 2 3 4 5 6 7... 100000
           |   | |
           v   v v
           G   G G
           |
           v
           G

*/
