#include <stdio.h>
#include <stdlib.h>
#include "charStack.h"

//#define DEBUG_PRINT
void initStack(struct charStack *stack) {
    const unsigned char initialSize = 100;
    #ifdef DEBUG_PRINT
        printf("Creating stack of size %u\n",initialSize);
    #endif
    stack->contents=malloc(initialSize);
    if (!stack->contents) {
        fprintf(stderr,"Malloc fail for %u bytes! Exiting...\n",initialSize);
        exit(1);
    }
    stack->maxSize=initialSize;
    stack->currentIndex=0;
}

void verifySize(struct charStack *stack) {
    if (stack->currentIndex < stack->maxSize) return;
    #ifdef DEBUG_PRINT
        printf("Enlarging stack from %lu bytes to ",stack->maxSize);
    #endif
    stack->maxSize = (stack->maxSize << 1) + 1;
    stack->contents = realloc(stack->contents,stack->maxSize);
    if (!stack->contents) {
        fprintf(stderr, "Malloc fail for %lu bytes! Exiting...\n", stack->maxSize);
        exit(1);
    }
    #ifdef DEBUG_PRINT
        printf("%lu bytes.\n",stack->maxSize);
    #endif
}

unsigned long charsOnStack(struct charStack *stack) {
    return stack->currentIndex;
}

char charStackEmpty(struct charStack *stack) {
    return !stack->currentIndex;
}

void charPush(struct charStack *stack, char c) {
    verifySize(stack);
    stack->contents[stack->currentIndex++]=c;
    #ifdef DEBUG_PRINT
        printf("Pushed %d onto the stack\n",c);
    #endif
}

char charPop(struct charStack *stack) {
    if (charStackEmpty(stack)) {
        fprintf(stderr,"Programmer error in something using a stack (pop an empty stack)!\n");
        exit(1);
    }
    #ifdef DEBUG_PRINT
        printf("Popped %d from the stack\n",stack->contents[stack->currentIndex-1]);
    #endif
    return stack->contents[--(stack->currentIndex)];
}

void destroyStack(struct charStack *stack) {
    free(stack->contents);
}

/**
 * Program to test this implementation of a stack
* /
int main() {
    struct charStack s;
    initStack(&s);
    printf("Stack is empty: %s\n",(charStackEmpty(&s))?"true":"false");
    charPush(&s,0);
    printf("Stack is empty: %s\n",(charStackEmpty(&s))?"true":"false");
    charPush(&s,1);
    charPush(&s,2);
    charPush(&s,3);
    charPop(&s);
    charPop(&s);
    charPop(&s);
    charPush(&s,4);
    charPop(&s);
    charPush(&s,5);
    charPush(&s,6);
    charPop(&s);
    charPop(&s);
    charPop(&s);
    printf("Stack is empty: %s\n",(charStackEmpty(&s))?"true":"false");
    destroyStack(&s);
    printf("\n");
    for(initStack(&s);charsOnStack(&s)<1000;charPush(&s,charsOnStack(&s))) if (charStackEmpty(&s) && charsOnStack(&s)) fprintf(stderr, "charStackEmpty doesn't work idiot\n");
    destroyStack(&s);
}*/
