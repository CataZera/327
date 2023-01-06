#ifndef StacksAreBetterThanQueuesYo
#define StacksAreBetterThanQueuesYo

/**
 * A struct to store chars in a stack.
 * This struct's internal components should never be modified directly, only by the methods initStack(), charPush(), charPop(), and destroyStack()
*/
struct charStack {
    char* contents;
    unsigned long maxSize;
    unsigned long currentIndex;
};

//Public Methods
/**
 * Initializes the values inside the passed in struct to allow for pushing and popping.
 * Initially the stack is empty.
 */
void initStack(struct charStack *stack);

/**
 * Returns true if the stack is empty, and false otherwise. This must be called before using charPop().
 */
char charStackEmpty(struct charStack *stack);

/**
 * Returns the number of elements on the stack (0 after initializing, 1 after a push, 0 after pop, etc)
 */
unsigned long charsOnStack(struct charStack *stack);

/**
 * Puts the specified character on top of the stack.
 */
void charPush(struct charStack *stack, char c);

/**
 * Pops the top of the stack off, returning it.
 */
char charPop(struct charStack *stack);

/**
 * Frees the memory stored by the chunk. After calling this method, no method should be called using this stack.
 * Note: charStackEmpty and getStackIndex will work, but cmon you're going to destroy a stack and then use it? Don't. Just no.
 */
void destroyStack(struct charStack *stack);

//Private internal method. Used to increase the size of the stack's memory when it would otherwise run out.
void verifySize(struct charStack *stack);

#endif
