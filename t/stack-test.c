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


    ok(stack_init(&stack, 10, sizeof(int)) == STACK_SUCCESS, "stack_init");
    ok(stack.top == NULL, "top is NULL");


    /*** push ***/
    tmp = 1;
    ok(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(1)");
    ok(*(int*)stack.top == 1, "top is 1");

    tmp = 2;
    ok(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(2)");
    ok(*(int*)stack.top == 2, "top is 2");

    tmp = 3;
    ok(stack_push(&stack, &tmp) == STACK_SUCCESS, "stack_push(3)");
    ok(*(int*)stack.top == 3, "top is 3");
    /*** push end ***/

    /*** pop ***/
    ok(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(3)");
    ok(*(int*)stack.top == 2, "top is 2");

    ok(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(2)");
    ok(*(int*)stack.top == 1, "top is 1");

    ok(stack_pop(&stack) == STACK_SUCCESS, "stack_pop(1)");
    ok(stack.top == NULL, "top is NULL");

    ok(stack_pop(&stack) == STACK_RANGE_ERROR, "can't do stack_pop(NULL)");
    /*** pop end ***/

    ok(stack_destruct(&stack) == STACK_SUCCESS, "stack_destruct");


    return EXIT_SUCCESS;
}
