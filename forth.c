/*
 * forth.c - api of forth
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-01.
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

    /* initialize error variables */
    interp->errid = FORTH_ERR_NOERR;

    /* initialize source code storage */
    interp->src = CAST(char*, malloc(SRC_DFL_BYTE));
    memset(interp->src, 0, SRC_DFL_BYTE);

    /* initialize forth operators */
    interp->word_def = CAST(
        ForthWord*, calloc(sizeof(ForthWord), 4)
    );
    word_init(interp->word_def);
    interp->word_pos = 0;

    forth_regist_word(interp, "+", forth_word_plus);
    forth_regist_word(interp, "-", forth_word_minus);
    forth_regist_word(interp, "*", forth_word_multiply);
    forth_regist_word(interp, "/", forth_word_divide);

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
        d_printf("pop![%s]\n", AC_TOP_WORD(interp)->tok_str.str);
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
                d_printf("dispatch![%s]\n", word->tok_str.str);

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
        d_printf("[%s] - eval begin.\n", interp->src);

        // get words as possible.
        forth_run_src(interp);

        d_printf("[%s] - eval end.\n", interp->src);

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


void
forth_die(ForthInterp *interp, const char *msg, forth_err_id id)
{
    if ((int)id != -1) interp->errid = id;
    forth_perror(interp, msg);
    forth_exit(interp, EXIT_FAILURE);
}


void
forth_error(ForthInterp *interp, const char *msg, forth_err_id id)
{
    fprintf(stderr, "[error]:: %s\n", msg);
    forth_die(interp, "forth_error", id);
}


void
forth_perror(ForthInterp *interp, const char *msg)
{
    if (msg != NULL)
        fprintf(stderr, "%s: ", msg);

    switch (interp->errid) {
        case FORTH_ERR_INIT:
            fputs("failed to initialize interpreter struct...", stderr);
            break;
        case FORTH_ERR_SIG:
            fputs("receive error signals...exit", stderr);
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


/* parser */

// get token, convert it, push it to interp->word.
bool
forth_get_word(ForthInterp *interp)
{
    char token[interp->max_word_len];    // c99
    ForthWord word;

    word_init(&word);

    // parse interp->src
    // NOTE: token will be allocated.
    bool success = forth_get_token_from_src(interp, token, interp->max_word_len);

    if (success) {
        d_printf("read [%s].\n", token);
        // convert: char* -> ForthWord
        forth_token2word(interp, token, &word);
        // push it.
        d_printf("push![%s]\n", token);
        stack_push(interp->word_stack, &word);
    }

    // don't free pushed word! stack_push() copies only all bytes of struct.
    // word_destruct(&word);

    // to trap EOF outside of this function,
    // this func must return this value.
    // (do not die in this func!)
    return success;

    // 'word' at stack area is free()d but copied to interp->word_stack.
}


/* word */

// NOTE: copy the address of tok_str.
void
forth_regist_word(ForthInterp *interp, const char *tok_str, word_func_t func)
{
    // NOTE: copy the address of tok_str.
    word_set_tok_str(interp->word_def + interp->word_pos, tok_str);
    interp->word_def[interp->word_pos].func = func;
    interp->word_def[interp->word_pos].type = WORD_FUNC;

    interp->word_pos++;
}


char*
forth_word_as_str(ForthInterp *interp, ForthWord *word)
{
    if (word->tok_str.str == NULL) {
        forth_uneval_word(interp, word);
    }
    return word->tok_str.str;
}


// evaluate word->tok_str.
// NOTE: word->type and word->tok_str must be set.
void
forth_eval_word(ForthInterp *interp, ForthWord *word)
{
    if (word->tok_str.str == NULL) return;

    if (word->type == WORD_DIGIT) {
        if (! word->digitval.is_set) {
            ASSERT(interp, word->tok_str.str != NULL);
            char *err = NULL;

            digit_t d = atod(word->tok_str.str, 10, &err);
            if (err != NULL) {
                fprintf(stderr, "%s: ", word->tok_str.str);
                forth_die(interp, "atod", FORTH_ERR_CONVERT_FAILED);
            }

            word_set_digit(word, d);
            d_printf("eval: %s -> %f\n", word->tok_str.str, word->digitval.digit);
        }
    }
    else if (word->type == WORD_STRING) {
        if (word->strval.str == NULL) {
            size_t len = strlen(word->tok_str.str);
            char str[len + 1];    // evaluated string.

            // empty string.
            if (STREQ(word->tok_str.str, "\"\"")) {
                word_set_str(word, "");
                return;
            }

            char *begin, *end, *cur_srch_pos;
            // set inside double quotes
            cur_srch_pos = begin = word->tok_str.str + 1;
            end = word->tok_str.str + word->tok_str.len - 2;    // remember null byte.

            // original code from parser.c
            while (1) {
                end = strchr(cur_srch_pos, '"');

                /* not found '"' */
                if (end == NULL) {
                    // process can't reach this block
                    // because parser checks this when parsing.
                    forth_die(interp, "forth_eval_word", FORTH_ERR_UNCLOSED_STR);
                }
                /* found it */
                else if (*(end - 1) != '\\') {    // if not escaped string.
                    strncpy(str, begin, end - begin);
                    str[end - begin] = '\0';
                    word_set_str_copy(word, str);

                    return;
                }
                /* found but it was escaped. search again. */
                else {
                    // TODO escape sequence.
                    cur_srch_pos = end;
                }
            }
        }
    }
    else if (word->type == WORD_FUNC) {
        forth_error(interp, "tried to convert word func to string.", FORTH_ERR_CONVERT_FAILED);

        // no strict? (in Perl)
        // word_set_str_copy(word, WORD_FUNC_STR);
    }
    else if (word->type == WORD_UNDEF) {
        forth_error(interp, "tried to convert undefined word to string.", FORTH_ERR_CONVERT_FAILED);

        // no strict? (in Perl)
        // word_set_str_copy(word, "");
    }
    else {
        // never reach this block
        ASSERT(interp, 0);
    }
}


void
forth_uneval_word(ForthInterp *interp, ForthWord *word)
{
    char tmp[interp->max_word_len];    // c99

    if (word->tok_str.str != NULL) return;


    if (word->type == WORD_DIGIT) {
        ASSERT(interp, word->digitval.is_set);

        if (! dtoa(word->digitval.digit, tmp, interp->max_word_len, 10))
            forth_die(interp, "dtoa", FORTH_ERR_CONVERT_FAILED);

        word_set_tok_str_copy(word, tmp);
    }
    else if (word->type == WORD_STRING) {
        // TODO
    }
    else if (word->type == WORD_FUNC) {
        forth_error(interp, "tried to convert word func to string.", FORTH_ERR_CONVERT_FAILED);

        // no strict? (in Perl)
        // word_set_str_copy(uneval, WORD_FUNC_STR);
    }
    else if (word->type == WORD_UNDEF) {
        forth_error(interp, "tried to convert undefined word to string.", FORTH_ERR_CONVERT_FAILED);

        // no strict? (in Perl)
        // word_set_str_copy(uneval, WORD_FUNC_STR);
    }
    else {
        // never reach this block
        ASSERT(interp, 0);
    }
}


/* token */

word_type
forth_get_word_type(ForthInterp *interp, const char *token)
{
    if (forth_get_word_def(interp, token) != NULL)
        return WORD_FUNC;
    if (is_string(token))
        return WORD_STRING;
    if (is_digit(token))
        return WORD_DIGIT;

    return WORD_UNDEF;
}


ForthWord*
forth_get_word_def(ForthInterp *interp, const char *token)
{
    // TODO bsearch() sorted array.
    for (size_t i = 0; i < interp->word_pos; i++) {
        // XXX interp->word_def[i].token.str can't be NULL?
        if (STREQ(token, interp->word_def[i].tok_str.str))
            return interp->word_def + i;
    }
    return NULL;
}


/* util */

// convert token to word.
void
forth_token2word(ForthInterp *interp, const char *token, ForthWord *word)
{
    // identify token's type.
    word->type = forth_get_word_type(interp, token);
    d_printf("word type: %d\n", word->type);

    // copy parsed string to tok_str.
    word_set_tok_str_copy(word, token);

    // find token's func.
    if (word->type == WORD_FUNC) {
        ForthWord *w = forth_get_word_def(interp, token);
        if (w == NULL) return;
        word->func = w->func;
    }
    else {
        word->func = WORD_NULL_FUNC;
    }
}
