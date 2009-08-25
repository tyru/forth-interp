#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../struct.h"
#include "../stack.h"


static ForthStack stack;

void
destruct(void)
{
    stack_destruct(&stack);
}


int main(void)
{
    int tmp = 0;

    atexit(destruct);


    OK(stack_init(&stack, 10, sizeof(int)) == STACK_SUCCESS, "stack_init");
    OK(stack.top == NULL, "top is NULL");


    /*** push ***/
    tmp = 1;
    OK(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(1)");
    OK(*(int*)stack.top == 1, "top is 1");

    tmp = 2;
    OK(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(2)");
    OK(*(int*)stack.top == 2, "top is 2");

    tmp = 3;
    OK(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(3)");
    OK(*(int*)stack.top == 3, "top is 3");
    /*** push end ***/

    /*** pop ***/
    OK(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(3)");
    OK(*(int*)stack.top == 2, "top is 2");

    OK(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(2)");
    OK(*(int*)stack.top == 1, "top is 1");

    OK(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(1)");
    OK(stack.top == NULL, "top is NULL");

    OK(stack_pop(&stack) == STACK_RANGE_ERROR, "can't do stack_pop(NULL)");
    /*** pop end ***/

    OK(stack_destruct(&stack) == STACK_SUCCESS, "stack_destruct");


    return EXIT_SUCCESS;
}
