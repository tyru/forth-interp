#include <stdlib.h>
#include <string.h>

// FIXME:
// this makes 'conflicting types' error
// when including top of this file.
#include "stack.h"

#include "preproc.h"



stack_ret
stack_init(ForthStack *stack, size_t size, size_t elem_size)
{
    stack->stack = REINTERP_CAST(void**, calloc(sizeof(void*), size));
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
    if (stack->top == NULL || stack->cur_pos < 0) return STACK_RANGE_ERROR;

    if (stack->cur_pos == 0) {
        free(stack->top);
        stack->top = NULL;
    }
    else {
        free(stack->top);
        stack->cur_pos--;
        stack->top = stack->stack[stack->cur_pos];
    }

    return STACK_SUCCESS;
}


// stack_ret
// stack_copy(ForthStack *dest, ForthStack *src)
// {
// }


stack_ret
stack_destruct(ForthStack *stack)
{
    while (stack->cur_pos > 0) {
        stack_ret ret = stack_pop(stack);
        if (ret != STACK_SUCCESS) return ret;
    }
    free(stack->stack);

    return STACK_SUCCESS;
}


