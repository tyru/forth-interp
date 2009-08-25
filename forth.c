/*
 * forth.c - api of forth
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-26.
 *
 */

#include "forth.h"

// data
#include "preproc.h"
// api
#include "word.h"
#include "parser.h"

#include "util.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#define _GNU_SOURCE
#include <getopt.h>


/* api */

void
forth_init(ForthInterp *interp)
{
    /* don't use forth_malloc() in this func. */

    /* initialize source code storage */
    interp->src = REINTERP_CAST(char*, malloc(SRC_DFL_BYTE));
    memset(interp->src, 0, SRC_DFL_BYTE);


    /* initialize constant and counter */
    interp->max_src_len  = SRC_DFL_BYTE;
    interp->max_line_len = SRC_DFL_LINEBYTE;
    interp->max_word_len = SRC_DFL_WORDBYTE;
    interp->src_len = 0;
    interp->cur_pos = 0;
    interp->errno = 0;


    /* initialize forth operators */
    interp->words = REINTERP_CAST(
        ForthWord*, calloc(sizeof(ForthWord), 4)
    );
    interp->word_pos = 0;

    forth_regist_word(interp, "+", forth_word_plus);
    forth_regist_word(interp, "-", forth_word_minus);
    forth_regist_word(interp, "*", forth_word_multiply);
    forth_regist_word(interp, "/", forth_word_divide);


    // TODO
    // stack_init(interp->stack, STACK_DFL_NUM, sizeof(char*));
}


void
forth_regist_word(ForthInterp *interp, char *name, forth_word_t func)
{
    interp->words[interp->word_pos].name = name;
    interp->words[interp->word_pos].func = func;
    interp->word_pos++;
}


// NOTE: it's ok to destruct interp more than twice.
void
forth_destruct(ForthInterp *interp)
{
    if (interp->src != NULL) {
        free(interp->src);
        interp->src = NULL;
    }
    if (interp->words != NULL) {
        free(interp->words);
        interp->words = NULL;
    }
}


void
forth_getopt(ForthInterp *interp, int *argc, char **argv)
{
    static struct option long_opts[] = {
        {"help", 0, NULL, 'h'}
    };
    int opt_index = 0;
    int c;

    while ((c = getopt_long(*argc, argv, "h", long_opts, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                puts("May the forth be with you.");
                forth_exit(interp, EXIT_SUCCESS);
                /* NOTREACHED */

            case '?':
                forth_die(interp, "getopt_long", EXIT_FAILURE);
        }
    }
}


void
forth_repl(ForthInterp *interp)
{
    char linebuf[interp->max_line_len];    // c99

    while (1) {
        printf(REPL_PROMPT_STR);

        // clear buffer each line.
        memset(linebuf, 0, interp->max_line_len);
        if (fgets(linebuf, interp->max_line_len, stdin) == NULL)
            break;

        // set linebuf as source code.
        forth_set_src(interp, linebuf);
        // execute.
        forth_exec_src(interp);
    }
}


void
forth_set_src(ForthInterp *interp, char *src)
{
    if (strlen(src) >= interp->max_src_len) {
        interp->errno = FORTH_ERR_OVERFLOW;
        forth_die(interp, "forth_set_src", EXIT_FAILURE);
    }

    // clear previous string.
    forth_clear_src(interp);
    // copy string.
    strncpy(interp->src, src, interp->max_src_len);
    // set string length.
    interp->src_len = strlen(src);
    // init current reading position.
    interp->cur_pos = 0;
}


void
forth_clear_src(ForthInterp *interp)
{
    memset(interp->src, 0, interp->max_src_len);
    interp->src_len = 0;
}


void
forth_exec_src(ForthInterp *interp)
{
    char word[interp->max_word_len];    // c99
    memset(word, 0, interp->max_word_len);

    d_printf("debug: [%s]\n", interp->src);

    bool success = forth_get_word_from_src(interp, word, interp->max_word_len);
    if (! success) {
        forth_die(interp, "forth_get_word_from_src", EXIT_FAILURE);
    }
    d_printf("debug: read [%s].\n", word);
}


void*
forth_malloc(ForthInterp *interp, size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        interp->errno = FORTH_ERR_ALLOC;
        forth_die(interp, "malloc", EXIT_FAILURE);
    }
    return ptr;
}


void
forth_die(ForthInterp *interp, const char *msg, int status)
{
    forth_perror(interp, msg);
    forth_exit(interp, status);
}


void
forth_perror(ForthInterp *interp, const char *msg)
{
    fputs(msg, stderr);
    switch (interp->errno) {
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

        default:
            fputs("No error found but forth_perror() was called.", stderr);
            break;
    }
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


