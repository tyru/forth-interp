#ifndef STACK_H
#define STACK_H

#include "type.h"


// this is used by only this file and stack.c
enum stack_ret {
    STACK_SUCCESS = 1,
    STACK_ARG_ERROR,      // some arguments are wrong.
    STACK_RANGE_ERROR,    // can't push or pop no more.
    STACK_ALLOC_ERROR,    // can't allocate memory.
};
// this enum is used by only stack.h and stack.c
typedef enum stack_ret stack_ret;


struct ForthStack {
    void **stack;     // stack
    void *top;        // top of element
    int cur_pos;      // current position of stacks
    int max_len;      // max length of stacks
    int elem_size;    // size of one element
};


stack_ret
stack_init(ForthStack *stack, size_t size, size_t elem_size);

stack_ret
stack_push(ForthStack *stack, void *val);

stack_ret
stack_pop(ForthStack *stack);

// stack_ret
// stack_copy(ForthStack *dest, ForthStack *src);

stack_ret
stack_destruct(ForthStack *stack);





#endif /* STACK_H */
