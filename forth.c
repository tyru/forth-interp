/*
 * forth.c - api of forth
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include "forth.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#define _GNU_SOURCE
#include <getopt.h>

#include <alloca.h>
#include <ctype.h>



/* api */

void
forth_init(ForthInterp *interp)
{
    /* don't use forth_malloc_noret() in this func. */

    /* initialize constant and counter */
    interp->max_src_len  = SRC_DFL_BYTE;
    interp->max_line_len = SRC_DFL_LINEBYTE;
    interp->max_word_len = SRC_DFL_WORDBYTE;
    interp->src_len = 0;
    interp->cur_pos = 0;

    /* initialize error variables */
    interp->errno = FORTH_ERR_NOERR;

    /* initialize source code storage */
    interp->src = CAST(char*, malloc(SRC_DFL_BYTE));
    memset(interp->src, 0, SRC_DFL_BYTE);

    /* initialize forth operators */
    interp->word_def = CAST(
        ForthWord*, calloc(sizeof(ForthWord), 4)
    );
    interp->word_pos = 0;

    forth_regist_word(interp, "+", forth_word_plus);
    forth_regist_word(interp, "-", forth_word_minus);
    forth_regist_word(interp, "*", forth_word_multiply);
    forth_regist_word(interp, "/", forth_word_divide);

    /* initialize forth stacks (for ForthWord) */
    stack_init(&(interp->word_stack), STACK_DFL_NUM, sizeof(ForthWord));

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
    if (interp->word_stack.stack == NULL)
        forth_die(interp, "forth_init", FORTH_ERR_ALLOC);
}


// NOTE: it's ok to destruct interp more than twice.
void
forth_destruct(ForthInterp *interp)
{
    // free the (void*) addresses.
    while (interp->word_stack.top != NULL) {
        word_destruct(CAST(ForthWord*, interp->word_stack.top));
        stack_pop(&(interp->word_stack));
    }
    // free the pointer.
    stack_destruct(&(interp->word_stack));

    FREE(interp->src);
    FREE(interp->word_def);
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
                forth_die(interp, "getopt_long", -1);
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
        interp->errno = FORTH_ERR_OVERFLOW;
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
forth_run_src(ForthInterp *interp)
{
    int i;
    // copy original source code.
    char orig_src[interp->src_len + 1];
    strncpy(orig_src, interp->src, interp->src_len + 1);
    // line
    char **lines;

    // allocate char** pointers.
    size_t line_num = strcount(orig_src, '\n') + 1;
    lines = alloca(line_num * sizeof(char*));
    // allocate char* strings (interp->max_line_len bytes per line).
    for (i = 0; (size_t)i < line_num; i++) {
        lines[line_num] = alloca(interp->max_line_len);
        memset(lines[line_num], 0, interp->max_line_len);
    }

    // split each line into lines.
    i = 0;
    char *saveptr;
    lines[i++] = strtok_r(orig_src, "\n", &saveptr);    // this destroys orig_src.
    while ((lines[i++] = strtok_r(NULL, "\n", &saveptr)) != NULL)
        ;

    // process each line.
    for (i = 0; lines[i] != NULL; i++) {
        // set source which doesn't contain newline.
        size_t len = strlen(lines[i]);
        strncpy(interp->src, lines[i], len + 1);
        d_printf("[%s] - eval begin.\n", interp->src);

        // get words as possible.
        while (1) {
            bool success = forth_get_word(interp);    // get and push the word.
            if (interp->errno == FORTH_ERR_EOF)    // eof
                break;
            if (! success)    // other error
                forth_die(interp, "forth_get_word", -1);

            ForthWord *word = CAST(ForthWord*, interp->word_stack.top);
            d_printf("debug: read [%s].\n", word->name);

            // concerning comparison result, add volatile.
            volatile word_func_t func;
            // dispatch process by its type.
            switch (word->type) {
                case WORD_UNDEF:
                    fprintf(stderr,
                            "%s: unknown token\n",
                            DEREF(ForthWord, interp->word_stack.top).name);
                    return;

                case WORD_FUNC:
                    // pop the top of word.
                    func = word->func;

                    word_destruct(word);
                    stack_pop(&(interp->word_stack));

                    ASSERT(func != WORD_NULL_FUNC);
                    func(interp);

                    break;

                default:
                    /* nop */
                    break;
            }
        }
        d_printf("[%s] - eval end.\n", interp->src);

        // pop all stacks.
        while (interp->word_stack.top != NULL) {
            d_printf("pop stacks...[%s]\n",
                    DEREF(ForthWord, interp->word_stack.top).name);
            word_destruct(CAST(ForthWord*, interp->word_stack.top));
            stack_pop(&(interp->word_stack));
        }
    }
}


void*
forth_malloc_noret(ForthInterp *interp, size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        interp->errno = FORTH_ERR_ALLOC;
        forth_die(interp, "malloc", -1);
    }
    return ptr;
}


void
forth_die(ForthInterp *interp, const char *msg, forth_err_id id)
{
    if ((int)id != -1) interp->errno = id;
    forth_perror(interp, msg);
    forth_exit(interp, EXIT_FAILURE);
}


void
forth_perror(ForthInterp *interp, const char *msg)
{
    fprintf(stderr, "%s: ", msg);

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
        case FORTH_ERR_STACK_OVERFLOW:
            fputs("forth's stack overflow error", stderr);
            break;
        case FORTH_ERR_BAD_STRING:
            fputs("malformed string", stderr);
            break;
        case FORTH_ERR_BAD_DIGIT:
            fputs("malformed digit", stderr);
            break;
        case FORTH_ERR_ASSERT_FAILED:
            fputs("sorry, assertion was failed.", stderr);
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


/* stack */

void
forth_push_stack_noret(ForthInterp *interp, ForthStack *stack, void *val)
{
    stack_ret ret = stack_push(stack, val);
    if (ret != STACK_SUCCESS) { switch (ret) { // STACK_ARG_ERROR -> FORTH_ERR_ARGS
            case STACK_ARG_ERROR:
                interp->errno = FORTH_ERR_ARGS;
                break;

            // STACK_RANGE_ERROR -> FORTH_ERR_STACK_OVERFLOW
            case STACK_RANGE_ERROR:
                interp->errno = FORTH_ERR_STACK_OVERFLOW;
                break;

            // STACK_ALLOC_ERROR -> FORTH_ERR_ALLOC
            case STACK_ALLOC_ERROR:
                interp->errno = FORTH_ERR_ALLOC;
                break;

            default:
                /* STACK_SUCCESS is not handled */
                break;
        }

        forth_die(interp, "stack_push", -1);
    }
}


/* parser */

// get token, convert it, push it to interp->word.
bool
forth_get_word(ForthInterp *interp)
{
    DECL_TOKEN(token);
    DECL_WORD(word);


    // parse interp->src
    bool success = forth_get_token_from_src(interp, &token, interp->max_word_len);

    if (success) {
        // convert: ForthToken -> ForthWord
        // NOTE: copying the address of token.name
        if (! forth_token_to_word(interp, &word, &token))
            forth_die(interp, "forth_token_to_word", FORTH_ERR_ALLOC);

        // push it.
        stack_push(&(interp->word_stack), &word);
    }

    return success;
}


/* word */

void
forth_regist_word(ForthInterp *interp, const char *name, word_func_t func)
{
    interp->word_def[interp->word_pos].name = CAST(char*, name);
    interp->word_def[interp->word_pos].func = func;
    interp->word_pos++;
}


/* token */

token_type
forth_get_token_type(ForthInterp *interp, const char *token)
{
    // TODO bsearch() sorted array.
    for (size_t i = 0; i < interp->word_pos; i++) {
        if (STREQ(token, interp->word_def[i].name))
            return TOKEN_FUNC;
    }
    if (is_string(token)) return TOKEN_STRING;
    if (is_digit(token)) return TOKEN_DIGIT;

    return TOKEN_UNDEF;
}


word_func_t
forth_get_token_func(ForthInterp *interp, const char *token)
{
    // TODO bsearch() sorted array.
    for (size_t i = 0; i < interp->word_pos; i++) {
        if (STREQ(token, interp->word_def[i].name))
            return interp->word_def[i].func;
    }
    return WORD_NULL_FUNC;
}


/* util */

// this returns always true.
bool
forth_token_to_word(ForthInterp *interp, ForthWord *word, ForthToken *token)
{
    word->name = token->name;
    word->type = token_type2word_type(token->type);

    if (word->type == TOKEN_FUNC) {
        word->func = forth_get_token_func(interp, token->name);
    }
    else {
        word->func = WORD_NULL_FUNC;
    }

    return true;
}
