//
// Created by Cata on 4/2/2020.
//
#include "bfHashtable.h"
#include "../tinyHelper.h"
#include "../simplicity.h"
#include <stdlib.h>

/**
 *
 * @param bf
 * @param size
 */
void init_table(struct hashtable* bf, unsigned long size){
    bf->hash = calloc(size, sizeof(struct hashtableNode*));
    if (bf->hash == NULL) printf("size of %lu failed to allocate the data\n", size);
    bf->size = size;
    bf->numElements=0;
    bf->numDuplicates = 0;
}


static inline unsigned long addToHash(unsigned long hash, unsigned long next) {
    //hash=((hash<<6)-hash)+ data->wasteStock[i];
    //return (hash<<8)-hash+next;
    return (hash<<6)-(hash<<3)-hash+next;
    //return (hash<<6)-(hash<<1)-hash+next;
}

/**
 *
 * @param data
 * @return
 */
unsigned long hash(struct gamestate* data) {
    unsigned long hash = 5381; //noo 46
    unsigned char i, j;
    for (i = 0; data->wasteStock[i] != -2; i++) {
        hash=addToHash(hash,data->wasteStock[i]);
    }
    for (i = 0; i < 4; i++){
        hash=addToHash(hash,data->foundations[i]);
    }
    for (i = 0; i < 7; i++) {
        for (j=0; data->tableau[i*21+j]!=-2; j++)
        hash=addToHash(hash,data->tableau[i*21+j]);
    }
    return hash;
}


/**
 *
 * @param bf
 * @param data
 * @param from
 * @param moveID
 * @return
 */
struct hashtableNode* add_to_table(struct hashtable* bf, struct gamestate* data, struct hashtableNode* from, char moveID){
    unsigned long h = hash(data) % bf->size;
    struct hashtableNode* find = bf->hash[h];
    while(find) {
        if(stateEquals(find->state, data)){
            bf->numDuplicates++;
            return 0;
        } else find = find->nextInList;  //Not fully sure this is correct but I am unsure how else to move to next
    }
    struct hashtableNode* newNode = malloc(sizeof(struct hashtableNode));
    newNode->from = from;
    newNode->state = data;
    newNode->moveID = moveID;
    newNode->nextInList = bf->hash[h];
    if (from){newNode->order = from->order + 1;}
    else newNode->order = 0;
    bf->hash[h] = newNode;
    bf->numElements++;
    return newNode;
}

/**
 *
 * @param state1
 * @param state2
 * @return
 *  1 if both states are a match so the hashtable doesn't add to it
 */
char stateEquals(struct gamestate* state1, struct gamestate* state2){
    unsigned i, j, counter1=0, counter2=0;
    for (i=0; i<7; i++){
        for (j = 0; j < 21; j++){
            if (state1->tableau[i*21+j] == -2) {
                if (state2->tableau[i*21+j] == -2) break;
                else return 0;
            } else if (state2->tableau[i*21+j] == -2) {
                return 0;
            }
            if (state1->tableau[i*21+j] != state2->tableau[i*21+j]) return 0;
        }
    }

    for (counter1=0; state1->wasteStock[counter1]!=-2;counter1++);
    for (counter2=0; state2->wasteStock[counter2]!=-2;counter2++);
    if (counter1 != counter2) return 0;
    for (i=0; i<counter1; i++) if (state1->wasteStock[i]!=state2->wasteStock[i]) return 0;

    for (i = 0; i < 4; i++){
        if (state1->foundations[i] != state2->foundations[i]) return 0;
    }
    return 1;
}

/**
 * Frees the hash table and all memory it owns
 * Note that by giving a pointer to a game state to the hashtable,
 * you must ensure that you will not use it after freeing the hashtable,
 * and that it is a pointer to the heap that may be leagally freed.
 */
void freeHashTable(struct hashtable* bf) {
    //Free all the nodes and their strings
    unsigned i;
    for (i=0; i < bf->size; i++) {
        struct hashtableNode *nextNode = bf->hash[i];
        struct hashtableNode *lastNode; //cannot free and then find the next node, so we must find the next node and then free the previous
        while (nextNode) {
            lastNode=nextNode;
            nextNode=nextNode->nextInList;
            free(lastNode->state);
            free(lastNode);
        }
    }
    //Free the table
    free(bf->hash);
    bf->size=0;
}
void printSlotsOccupiedHashTable(struct hashtable* bf) {
    //Free all the nodes and their strings
    unsigned i;
    for (i=0; i < 1000; i++) {
        struct hashtableNode *nextNode = bf->hash[i];
        unsigned long numNodes = 0;
            while (nextNode) {
            numNodes++;
            nextNode=nextNode->nextInList;
        }
    printf("%lu ",numNodes);
    }
}

