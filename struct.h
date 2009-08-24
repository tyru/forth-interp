#ifndef STRUCT_H
#define STRUCT_H


#include "type.h"


/* struct */
typedef struct ForthStack_ {
    void **stack;     // stack
    void *top;        // top of element
    int cur_pos;      // current position of stacks
    int max_len;      // max length of stacks
    int elem_size;    // size of one element
} ForthStack;


typedef struct ForthInterp_ {
    char *code;              // source code
    int   code_len;          // source code string length
    uint   code_read_pos;    // read
    ForthStack stack;        // stack
} ForthInterp;



#endif /* STRUCT_H */
