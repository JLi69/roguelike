#include "structs.h"
#include <stdlib.h>

//STACK OPERATIONS
Stack* stack_create(void)
{
	Stack* s = (Stack*)malloc(sizeof(Stack));
	s->size = 0;
	s->arrSize = 1;
	s->vals = (int*)malloc(sizeof(int));
	return s;
}
void stack_destroy(Stack *stack)
{
	free(stack->vals);	
	free(stack);
}

void stack_push(Stack *stack, int val)
{
	//Stack has enough space	
	if(stack->size < stack->arrSize)
		stack->vals[stack->size++] = val;
	//Stack ran out of space!
	else if(stack->size >= stack->arrSize)
	{
		//Temporary array
		int* temp = (int*)malloc(sizeof(int) * stack->arrSize);

		for(int i = 0; i < stack->arrSize; i++)
			temp[i] = stack->vals[i];
	
		//Enlarge the array		
		stack->arrSize *= 2;
		free(stack->vals);	
		stack->vals = (int*)malloc(stack->arrSize * sizeof(int));	
	
		//Copy the elements from the temp array into the enlarged array
		for(int i = 0; i < stack->arrSize; i++)
			stack->vals[i] = temp[i];
		
		//Add in the new element
		stack->vals[stack->size++] = val;

		free(temp);
	}
}

int stack_top(Stack *stack)
{
	return stack->vals[stack->size - 1];
}

int stack_pop(Stack *stack)
{
	//Stack is empty	
	if(stack->size == 0)
		return -2147483648; //Return this value to show that size is 0

	int formerTop = stack->vals[stack->size - 1];	
	//Decrease the size of the stack	
	stack->size--;

	//Determine if we need to deallocate memory
	if(stack->size * 4 < stack->arrSize)
	{
		if(stack->size == 0)
			return formerTop;

		int* temp = (int*)malloc(stack->size * sizeof(int));
		//Copy elements of stack into temp array
		for(int i = 0; i < stack->size; i++)
			temp[i] = stack->vals[i];

		free(stack->vals);
		stack->vals = (int*)malloc(sizeof(int) * stack->size);
		//Copy elements of temp back into stack
		for(int i = 0; i < stack->size; i++)
			stack->vals[i] = temp[i];

		//Reset the array size
		stack->arrSize = stack->size;

		free(temp);
	}

	return formerTop;
}

int stack_empty(Stack *stack)
{
	return stack->size == 0;
}
