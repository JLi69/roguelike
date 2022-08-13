/*
 * A bunch of convenient data structures to be used in the game
 * */

#ifndef INCLUDE_STRUCTS
//Stack data structure
typedef struct
{
	int* vals;
	int size, arrSize;
} Stack;

//Creates a stack
Stack* stack_create(void);
//Destroys the stack
void stack_destroy(Stack *stack);
//Puts a value onto the stack
void stack_push(Stack *stack, int val);
//Gives the value at the top of the stack
int stack_top(Stack *stack);
//Removes the value at the top of the stack and returns it
int stack_pop(Stack *stack);
//Convenient function to determine if stack is empty
int stack_empty(Stack *stack);
#endif

#define INCLUDE_STRUCTS
