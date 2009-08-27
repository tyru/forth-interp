#ifndef FORTH_H
#define FORTH_H

#include "type.h"

#include "word.h"

#include <stdio.h>
#include <stdbool.h>




// forth's variables
struct ForthVar {
    char    *name;
    value_t value;
};
typedef struct ForthVar ForthVar;


// set these error id by forth api.
enum forth_err_id {
    FORTH_ERR_NOERR = 0,
    FORTH_ERR_ARGS,
    FORTH_ERR_EOF,
    FORTH_ERR_ALLOC,
    FORTH_ERR_OVERFLOW,
};
typedef enum forth_err_id forth_err_id;



// interpreter
struct ForthInterp {
    char       *src;            // source src
    uint       max_src_len;     // allocated length of src. (default is SRC_MAX_BYTE)
    uint       src_len;         // source src string length
    uint       cur_pos;         // current reading position

    uint       max_line_len;    // allocated number of bytes each line
                                // (default is SRC_MAX_LINEBYTE)
    uint       max_word_len;    // allocated number of bytes each word
                                //(default is SRC_MAX_WORDBYTE)

    ForthStack *stack;          // stack

    ForthWord  *words;          // forth's word(functions?)
    uint       word_pos;

    ForthVar   *vars;

    int        errno;           // error id set by api
};





void
forth_init(ForthInterp *interp);

void
forth_regist_word(ForthInterp *interp, char *name, forth_word_t func);

void
forth_destruct(ForthInterp *interp);

void
forth_getopt(ForthInterp *interp, int *argc, char **argv);

void
forth_repl(ForthInterp *interp);

void
forth_set_src(ForthInterp *interp, char *src);

void
forth_clear_src(ForthInterp *interp);

void
forth_exec_src(ForthInterp *interp);

void*
forth_malloc(ForthInterp *interp, size_t size);

void
forth_die(ForthInterp *interp, const char *msg, int status);

void
forth_perror(ForthInterp *interp, const char *msg);

void
forth_exit(ForthInterp *interp, int status);

bool
forth_src_eof(ForthInterp *interp);


#endif /* FORTH_H */
