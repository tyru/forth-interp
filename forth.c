/*
 * forth.c - api of forth
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-09.
 *
 */

#include "forth.h"

#include "constant.h"
#include "digit.h"
#include "token.h"

#include "parser.h"
#include "util.h"
#include "stack.h"


#include <string.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <getopt.h>
#undef _GNU_SOURCE

#include <alloca.h>
#include <ctype.h>
#include <errno.h>

#include <stdarg.h>



/* api */

void
forth_init(ForthInterp *interp)
{
    /* don't use forth_malloc() in this func. */

    /* initialize constant and counter */
    interp->max_src_len  = SRC_DFL_BYTE;
    interp->max_line_len = SRC_DFL_LINEBYTE;
    interp->max_word_len = SRC_DFL_WORDBYTE;
    interp->src_len = 0;
    interp->cur_pos = 0;
    interp->debug = false;

    /* initialize error variables */
    interp->errid = FORTH_ERR_NOERR;

    /* initialize source code storage */
    interp->src = CAST(char*, malloc(SRC_DFL_BYTE));
    memset(interp->src, 0, SRC_DFL_BYTE);

    /* initialize word functions */
    forth_init_word(interp);

    /* initialize forth stacks (for ForthWord) */
    interp->word_stack = malloc(sizeof(ForthWord));
    stack_init(interp->word_stack, STACK_DFL_NUM, sizeof(ForthWord));

    /* check that all pointers is NOT NULL, etc. */
    forth_init_check(interp);
}


void
forth_init_check(ForthInterp *interp)
{
    if (interp->src == NULL)
        forth_die(interp, "forth_init", FORTH_ERR_ALLOC);
    if (interp->word_def == NULL)
        forth_die(interp, "forth_init", FORTH_ERR_ALLOC);
    if (interp->word_stack == NULL)
        forth_die(interp, "forth_init", FORTH_ERR_ALLOC);
    if (interp->word_stack->stack == NULL)
        forth_die(interp, "forth_init", FORTH_ERR_ALLOC);
}


// NOTE: it's ok to destruct interp more than twice.
void
forth_destruct(ForthInterp *interp)
{
    // free the (void*) addresses.
    while (AC_TOP_WORD(interp) != NULL) {
        word_destruct(AC_TOP_WORD(interp));
        stack_pop(interp->word_stack);
    }
    // free the pointer.
    stack_destruct(interp->word_stack);
    FREE(interp->word_stack);

    FREE(interp->src);
    FREE(interp->word_def);
}


void
forth_getopt(ForthInterp *interp, int *argc, char **argv)
{
    static const struct option long_opts[] = {
        {"help", 0, NULL, 'h'},
        {"debug", 0, NULL, 'd'},
    };
    int opt_index = 0;
    int c;

    while ((c = getopt_long(*argc, argv, "hd", long_opts, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                puts("May the forth be with you.");
                forth_exit(interp, EXIT_SUCCESS);
                break;
            case 'd':
                interp->debug = true;
                break;

            case '?':
                forth_exit(interp, EXIT_FAILURE);
        }
    }
}


void
forth_repl(ForthInterp *interp)
{
    char linebuf[interp->max_line_len];    // c99
    bool is_blank;

    while (1) {
        printf(REPL_PROMPT_STR);

        // clear buffer each line.
        memset(linebuf, 0, interp->max_line_len);
        if (fgets(linebuf, interp->max_line_len, stdin) == NULL)
            break;

        // continue if blank line.
        is_blank = true;
        size_t len = strlen(linebuf);
        for (size_t i = 0; i < len; i++) {
            // found non-space char
            if (! isspace(linebuf[i])) {
                is_blank = false;
                break;
            }
        }
        if (is_blank) continue;

        // set linebuf as source code.
        forth_set_src(interp, linebuf);
        // execute.
        forth_run_src(interp);
    }
}


void
forth_set_src(ForthInterp *interp, char *src)
{
    size_t len = strlen(src);
    if (len >= interp->max_src_len) {
        interp->errid = FORTH_ERR_OVERFLOW;
        forth_die(interp, "forth_set_src", -1);
    }

    // clear previous string.
    forth_clear_src(interp);
    // copy string.
    strncpy(interp->src, src, interp->max_src_len);
    // set string length.
    interp->src_len = len;
    // init current reading position.
    interp->cur_pos = 0;
}


void
forth_clear_src(ForthInterp *interp)
{
    memset(interp->src, 0, interp->max_src_len);
    interp->src_len = 0;
    interp->cur_pos = 0;
}


void
forth_clear_stack(ForthInterp *interp)
{
    while (AC_TOP_WORD(interp) != NULL) {
        // forth_uneval_word(interp, AC_TOP_WORD(interp))
        forth_debugf(interp, "pop![%s]\n", AC_TOP_WORD(interp)->tok_str.str);
        word_destruct(AC_TOP_WORD(interp));
        stack_pop(interp->word_stack);
    }
}


// get words and dispatch if that is WORD_FUNC.
void
forth_run_src(ForthInterp *interp)
{
    while (1) {
        bool success = forth_get_word(interp);    // get and push the word.
        if (interp->errid == FORTH_ERR_EOF)    // eof
            break;
        if (! success)    // other error
            forth_die(interp, "forth_get_word", -1);

        // convert current top word to string.
        ForthWord *word = AC_TOP_WORD(interp);

        // NOTE: concerning comparison result, add volatile.
        volatile word_func_t func;
        // dispatch process by its type.
        switch (word->type) {
            case WORD_UNDEF:
                fprintf(stderr,
                        "%s: unknown token\n",
                        word->tok_str.str);
                return;

            case WORD_FUNC:
                ASSERT(interp, word->func != WORD_NULL_FUNC);
                func = word->func;
                forth_debugf(interp, "dispatch![%s]\n", word->tok_str.str);

                // pop the func word
                word_destruct(word);
                stack_pop(interp->word_stack);

                // dispatch!
                func(interp);
                // error check
                if (interp->errid != FORTH_ERR_NOERR) {
                    forth_perror(interp, WORD_FUNC_STR);
                    interp->errid = FORTH_ERR_NOERR;
                    return;
                }

                break;

            default:
                /* nop */
                break;
        }
    }

    // show result
    if (AC_TOP_WORD(interp) != NULL) {
        printf("result was [%s]\n",
                forth_word_as_str(interp, AC_TOP_WORD(interp)));
    }
    else {
        puts("no result.");
    }
}


void
forth_run_src_each_line(ForthInterp *interp)
{
    int i;
    char **lines;


    // allocate char** pointers.
    size_t line_num = strcount(interp->src, '\n') + 1;
    lines = alloca(line_num * sizeof(char*));

    // split each line into lines.
    char *begin = interp->src;
    char *end;
    i = 0;
    while ((end = strchr(begin, '\n')) != NULL) {
        lines[i] = alloca(end - begin + 1);
        strncpy(lines[i], begin, end - begin);
        lines[i][end - begin] = '\0';
        // NOTE: assume newline is one byte.
        begin = end + 1;
        i++;
    }
    // NOTE: lines[i] may be allocated pointer.
    // alloca() frees its pointer at the end of func, though.
    lines[i] = NULL;

    ASSERT(interp, CAST(size_t, i + 1) == line_num);


    // process each line.
    for (i = 0; lines[i] != NULL; i++) {
        // set source which doesn't contain newline.
        size_t len = strlen(lines[i]);
        strncpy(interp->src, lines[i], len + 1);
        forth_debugf(interp, "[%s] - eval begin.\n", interp->src);

        // get words as possible.
        forth_run_src(interp);

        forth_debugf(interp, "[%s] - eval end.\n", interp->src);

        // pop all stacks.
        forth_clear_stack(interp);
    }
}


void*
forth_malloc(ForthInterp *interp, size_t size)
{
    void *ptr = malloc(size);

    if (ptr == NULL) {
        interp->errid = FORTH_ERR_ALLOC;
        forth_die(interp, "malloc", -1);
    }
    if (errno == ENOMEM) {
        interp->errid = FORTH_ERR_ALLOC;
        forth_die(interp, "malloc", -1);
    }

    return ptr;
}


// destroy interp and exit.
void
forth_die(ForthInterp *interp, const char *msg, forth_err_id id)
{
    if ((int)id != -1) interp->errid = id;
    forth_perror(interp, msg);
    forth_exit(interp, EXIT_FAILURE);
}


// raise error and die.
void
forth_error(ForthInterp *interp, const char *msg, forth_err_id id)
{
    fprintf(stderr, "[error]:: %s\n", msg);
    forth_die(interp, "forth_error", id);
}


void
forth_perror(ForthInterp *interp, const char *msg)
{
    fprintf(stderr, "error: ");
    if (msg != NULL)
        fprintf(stderr, "%s: ", msg);

    switch (interp->errid) {
        case FORTH_ERR_INIT:
            fputs("failed to initialize interpreter struct...", stderr);
            break;
        case FORTH_ERR_SIG:
            fputs("error signal was received...exit", stderr);
            break;

        case FORTH_ERR_ARGS:
            fputs("arguments error", stderr);
            break;
        case FORTH_ERR_EOF:
            fputs("EOF error", stderr);
            break;
        case FORTH_ERR_ALLOC:
            fputs("can't allocate memory.", stderr);
            break;
        case FORTH_ERR_OVERFLOW:
            fputs("overflow error", stderr);
            break;
        case FORTH_ERR_STACK_OVERFLOW:
            fputs("forth's stack overflow error", stderr);
            break;
        case FORTH_ERR_BAD_STRING:
            fputs("malformed string", stderr);
            break;
        case FORTH_ERR_BAD_DIGIT:
            fputs("malformed digit", stderr);
            break;
        case FORTH_ERR_CONVERT_FAILED:
            fputs("can't convert something to other types", stderr);
            break;
        case FORTH_ERR_NOT_FOUND_TOKEN:
            fputs("parser ended parsing but even any token had been not found", stderr);
            break;
        case FORTH_ERR_TOO_FEW_ARGS:
            fputs("word received too few arguments", stderr);
            break;
        case FORTH_ERR_NOT_IMPLEMENTED:
            fputs("not implemented error", stderr);
            break;

        case FORTH_ERR_ASSERT_FAILED:
            fputs("sorry, my assertion failed...", stderr);
            break;

        default:
            fputs("No error found but forth_perror() was called.", stderr);
            break;
    }

    fputs("\n", stderr);
}


void
forth_exit(ForthInterp *interp, int status)
{
    forth_destruct(interp);
    exit(status);
}


bool
forth_src_eof(ForthInterp *interp)
{
    return interp->cur_pos > interp->src_len;
}


int
forth_debugf(ForthInterp *interp, const char* format, ...)
{
    va_list ap;
    int result;

    if (! interp->debug) return 1;
    fputs("[debug]::", stderr);

    va_start(ap, format);
    result = vprintf(format, ap);
    va_end(ap);

    return result;
}

int
forth_debug(ForthInterp *interp, const char* msg)
{
    if (! interp->debug) return 1;
    fputs("[debug]::", stderr);
    fputs(msg, stderr);
    return 1;
}


