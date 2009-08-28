#ifndef FORTH_H
#define FORTH_H

#include "type.h"
#include "constant.h"

#include "parser.h"
#include "token.h"
#include "word.h"

#include "stack.h"
#include "util.h"


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
    FORTH_ERR_STACK_OVERFLOW,
    FORTH_ERR_UNCLOSED_STR,
    FORTH_ERR_BAD_STRING,
    FORTH_ERR_BAD_DIGIT,
    FORTH_ERR_ASSERT_FAILED,
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

    ForthStack word_stack;     // stack

    ForthWord  *word_def;       // forth's word definitions
    uint       word_pos;

    // ForthVar   *vars;

    int        errno;           // error id set by api
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
forth_run_src(ForthInterp *interp);

void*
forth_malloc_noret(ForthInterp *interp, size_t size);

void
forth_die(ForthInterp *interp, const char *msg, forth_err_id id);

void
forth_perror(ForthInterp *interp, const char *msg);

void
forth_exit(ForthInterp *interp, int status);

bool
forth_src_eof(ForthInterp *interp);


/* stack */
void
forth_push_stack_noret(ForthInterp *interp, ForthStack *stack, void *val);


/* parser */
bool
forth_get_word(ForthInterp *interp);


/* word */
void
forth_regist_word(ForthInterp *interp, const char *name, word_func_t func);

// TODO
// token_type
// forth_get_word_type(ForthInterp *interp, const char *token);

// TODO
// token_type
// forth_get_word_func(ForthInterp *interp, const char *token);


/* token */
token_type
forth_get_token_type(ForthInterp *interp, const char *token);

word_func_t
forth_get_token_func(ForthInterp *interp, const char *token);


/* util */

// this returns always true.
bool
forth_token_to_word(ForthInterp *interp, ForthWord *word, ForthToken *token);


#endif /* FORTH_H */
