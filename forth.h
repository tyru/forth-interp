#ifndef FORTH_H
#define FORTH_H

#include "type.h"

#include "word.h"

// this header is included from many headers.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>




// set these error id by forth api.
enum forth_err_id {
    FORTH_ERR_NOERR = 0,

    /* errors about internal */
    FORTH_ERR_INIT,
    FORTH_ERR_SIG,

    FORTH_ERR_TOO_FEW_ARGS,
    FORTH_ERR_NOT_IMPLEMENTED,
    FORTH_ERR_ARGS,
    FORTH_ERR_EOF,
    FORTH_ERR_ALLOC,
    FORTH_ERR_OVERFLOW,
    FORTH_ERR_STACK_OVERFLOW,
    FORTH_ERR_STACK_UNDERFLOW,

    FORTH_ERR_ASSERT_FAILED,

    /* errors about type or parser */
    FORTH_ERR_BAD_DIGIT,
    FORTH_ERR_BAD_STRING,
    FORTH_ERR_BAD_TOKEN,
    FORTH_ERR_NOT_FOUND_TOKEN,
    FORTH_ERR_CONVERT_FAILED,
    FORTH_ERR_UNCLOSED_STR,
    FORTH_ERR_UNEXPECTED_TYPE,
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

    ForthStack *word_stack;     // stack

    // TODO
    // manage words definitions with simple map.
    ForthWord  *word_def;       // forth's word definitions
    uint       word_pos;

    ForthWord  *vars;

    int        errid;           // error id set by api

    bool       debug;
};





void
forth_init(ForthInterp *interp);

void
forth_init_check(ForthInterp *interp);

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
forth_clear_stack(ForthInterp *interp);

// get words and dispatch if that is WORD_FUNC.
bool
forth_run_src(ForthInterp *interp);

void*
forth_malloc(ForthInterp *interp, size_t size);

void
forth_warning(ForthInterp *interp, const char *msg, forth_err_id id);

void
forth_die(ForthInterp *interp, const char *msg, forth_err_id id);

void
forth_error(ForthInterp *interp, const char *msg, forth_err_id id);

void
forth_perror(ForthInterp *interp, const char *msg);

void
forth_exit(ForthInterp *interp, int status);

bool
forth_src_eof(ForthInterp *interp);

int
forth_debugf(ForthInterp *interp, const char* format, ...);

int
forth_debug(ForthInterp *interp, const char* msg);



#endif /* FORTH_H */
