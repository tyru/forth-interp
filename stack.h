#ifndef STACK_H
#define STACK_H

#include "struct.h"


typedef enum stack_ret_t {
    STACK_SUCCESS = 1,
    STACK_ARG_ERROR,      // some arguments are wrong.
    STACK_RANGE_ERROR,    // can't push or pop no more.
    STACK_ALLOC_ERROR,    // can't allocate memory.
} stack_ret;



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
