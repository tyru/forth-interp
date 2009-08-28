/*
 * stack.c - stack implementation using heap (slow)
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include <stdlib.h>
#include <string.h>

// FIXME:
// this makes 'conflicting types' error
// when including top of this file.
#include "stack.h"

#include "util.h"



stack_ret
stack_init(ForthStack *stack, size_t size, size_t elem_size)
{
    // stack->stack = calloc(sizeof(void*), size);
    stack->stack = CAST(void**, malloc(sizeof(void*) * size));
    if (stack->stack == NULL) return STACK_ALLOC_ERROR;

    stack->top = NULL;
    stack->cur_pos = -1;
    stack->max_len = size;
    stack->elem_size = elem_size;

    return STACK_SUCCESS;
}


stack_ret
stack_push(ForthStack *stack, void *val)
{
    if (stack == NULL || val == NULL) return STACK_ARG_ERROR;
    if (stack->cur_pos >= stack->max_len) return STACK_RANGE_ERROR;

    void *ptr = malloc(stack->elem_size);
    if (ptr == NULL) return STACK_ALLOC_ERROR;

    // copy *val to *ptr. ('*ptr = *val' is wrong because ptr is 'void*'.)
    memmove(ptr, val, stack->elem_size);

    // push the address of value at heap.
    if (stack->cur_pos >= 0) {
        stack->cur_pos++;
    }
    else {
        stack->cur_pos = 0;
    }
    stack->stack[stack->cur_pos] = stack->top = ptr;

    return STACK_SUCCESS;
}


stack_ret
stack_pop(ForthStack *stack)
{
    if (stack == NULL) return STACK_ARG_ERROR;
    if (stack->top == NULL) return STACK_RANGE_ERROR;
    if (stack->cur_pos < 0) return STACK_RANGE_ERROR;

    FREE(stack->top);
    if (stack->cur_pos != 0) {
        stack->cur_pos--;
        stack->top = stack->stack[stack->cur_pos];
    }
    else {
        stack->cur_pos = -1;
        stack->top = NULL;
    }

    return STACK_SUCCESS;
}


// NOTE: this does NOT free() members under its struct.
stack_ret
stack_clear(ForthStack *stack)
{
    if (stack == NULL) return STACK_ARG_ERROR;
    if (stack->top == NULL || stack->cur_pos < 0) return STACK_RANGE_ERROR;

    while (stack->top != NULL) {
        stack_pop(stack);
    }

    return STACK_SUCCESS;
}


// stack_ret
// stack_copy(ForthStack *dest, ForthStack *src)
// {
// }


// NOTE: it's ok to destruct stack more than twice.
stack_ret
stack_destruct(ForthStack *stack)
{
    if (stack == NULL) return STACK_ARG_ERROR;
    if (stack->stack == NULL) return STACK_ARG_ERROR;

    // free the (void*) addresses.
    while (stack->cur_pos > 0) {
        stack_ret ret = stack_pop(stack);
        if (ret != STACK_SUCCESS) return ret;
    }
    // free the pointer.
    FREE(stack->stack);

    return STACK_SUCCESS;
}
