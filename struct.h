#ifndef STRUCT_H
#define STRUCT_H


#include "type.h"
#include "word.h"


/* struct */

// stack
typedef struct ForthStack_ {
    void **stack;     // stack
    void *top;        // top of element
    int cur_pos;      // current position of stacks
    int max_len;      // max length of stacks
    int elem_size;    // size of one element
} ForthStack;


// forth operators
typedef struct ForthWord_ {
    char            *name;
    forth_word_func func;
} ForthWord;


// variables
typedef struct ForthVar_ {
    char    *name;
    value_t value;
} ForthVar;


// interpreter
typedef struct ForthInterp_ {
    char       *src;            // source src
    uint       max_src_len;     // allocated length of src. (default is SRC_MAX_BYTE)
    uint       src_len;         // source src string length
    uint       cur_pos;         // current reading position

    uint       max_line_len;    // allocated number of bytes each line
                                // (default is SRC_MAX_LINEBYTE)
    uint       max_word_len;    // allocated number of bytes each word
                                //(default is SRC_MAX_WORDBYTE)

    ForthStack stack;           // stack
    ForthWord  *words;          // forth's word(functions?)
    ForthVar   *vars;

    int        errno;           // error id set by api
} ForthInterp;


// set these error id by forth api.
typedef enum forth_err_id_  {
    FORTH_ERR_ARGS = 1,
    FORTH_ERR_EOF,
    FORTH_ERR_ALLOC,
    FORTH_ERR_OVERFLOW,
} forth_err_id;



#endif /* STRUCT_H */
