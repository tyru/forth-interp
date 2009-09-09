/*
 * word.c - ForthWord api
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-09.
 *
 */

#include "word.h"

#include "word_def.h"

#include "forth.h"
#include "util.h"
#include "stack.h"

#include <stdlib.h>
#include <errno.h>
#include <alloca.h>





void
word_init(ForthWord *word)
{
    word->type = WORD_UNDEF;
    word->func  = WORD_NULL_FUNC;

    word->tok_str.str      = NULL;
    word->tok_str.capacity = -1;
    word->tok_str.len      = -1;

    word->strval.str      = NULL;
    word->strval.capacity = -1;
    word->strval.len      = -1;

    word->digitval.digit  = 0;
    word->digitval.is_set = false;
}


// new word
void
word_init_with_word(ForthWord *dest, ForthWord *src)
{
    word_init(dest);

    // digit
    word_set_digit(dest, src->digitval.digit);
    // str
    word_set_str_copy(dest, src->strval.str);
    // tok_str
    word_set_tok_str_copy(dest, src->tok_str.str);

    dest->type = src->type;
}


// new digit
void
word_init_with_digit(ForthWord *word, digit_t digit)
{
    word_init(word);
    word->type = WORD_DIGIT;
    word_set_digit(word, digit);
}


// new tok_str
void
word_init_with_tok_str(ForthWord *word, const char *tok_str)
{
    word_init(word);
    word_set_tok_str_copy(word, tok_str);
}


// new str
void
word_init_with_str(ForthWord *word, const char *str)
{
    word_init(word);
    word->type = WORD_STRING;
    word_set_str_copy(word, str);
}


void
word_destruct(ForthWord *word)
{
    FREE(word->tok_str.str);
    FREE(word->strval.str);
}


/* setter */

// tok_str
void
word_set_tok_str(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->tok_str.str = CAST(char*, str);

    word->tok_str.capacity = -1;    // not allocated!
    word->tok_str.len = len;
}


// tok_str copy
void
word_set_tok_str_copy(ForthWord *word, const char *str)
{
    if (str == NULL) return;
    size_t len = strlen(str);

    // TODO use realloc()
    word->tok_str.str = malloc(len + 1);
    if (! ALLOCATED(word->tok_str.str)) {
        FREE(word->tok_str.str);    // for safety
        return;
    }

    strcpy(word->tok_str.str, str);

    word->tok_str.capacity = len + 1;
    word->tok_str.len = len;
}


// str
void
word_set_str(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->strval.str = CAST(char*, str);
    word->strval.capacity = -1;    // not allocated!
    word->strval.len = len;
}


// str copy
void
word_set_str_copy(ForthWord *word, const char *str)
{
    if (str == NULL) return;
    size_t len = strlen(str);

    // TODO use realloc()
    word->strval.str = malloc(len + 1);
    if (! ALLOCATED(word->strval.str)) {
        FREE(word->strval.str);    // for safety
        return;
    }

    strcpy(word->strval.str, str);

    word->strval.capacity = len + 1;
    word->strval.len = len;
}


// digit
void
word_set_digit(ForthWord *word, digit_t digit)
{
    word->digitval.digit = digit;
    word->digitval.is_set = true;
}


/* word api for ForthInterp, etc. */

// called from forth_init().
void
forth_init_word(ForthInterp *interp)
{
    const int word_num = 5;

    interp->word_def = CAST(
        ForthWord*, calloc(sizeof(ForthWord), word_num)
    );
    word_init(interp->word_def);
    interp->word_pos = 0;

    // regist word funcs.
    forth_regist_word(interp, "+", forth_word_plus);
    forth_regist_word(interp, "-", forth_word_minus);
    forth_regist_word(interp, "*", forth_word_multiply);
    forth_regist_word(interp, "/", forth_word_divide);
    forth_regist_word(interp, ".", forth_word_print);
}


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


/* type conversion */

char*
forth_word_as_tok_str(ForthInterp *interp, ForthWord *word)
{
    // TODO
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
            forth_debugf(interp, "eval: %s -> %f\n", word->tok_str.str, word->digitval.digit);
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


// evaluate each member and set result to word->tok_str.
// NOTE: word->type and its value must be set.
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


// identify token.
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


// get the word defition.
ForthWord*
forth_get_word_def(ForthInterp *interp, const char *token)
{
    // TODO bsearch() sorted array.
    for (size_t i = 0; i < interp->word_pos; i++) {
        if (STREQ(token, interp->word_def[i].tok_str.str))
            return interp->word_def + i;
    }
    return NULL;
}


/* utility functions for word functions */

void
forth_pop_word(ForthInterp *interp, ForthWord *word)
{
    ForthWord *top = AC_TOP_WORD(interp);

    // copy to word
    word_init_with_word(word, top);

    if (top->tok_str.str != NULL) {
        forth_debugf(interp, "pop![%s]\n", top->tok_str.str);
    } else {
        forth_debug(interp, "pop!\n");
    }
    // pop
    word_destruct(top);
    stack_pop(interp->word_stack);
}


// - convert tok_str to digit.
// - check the top word's type.
void
forth_pop_str(ForthInterp *interp, char *str)
{
    if (str == NULL)
        forth_die(interp, "forth_pop_digit", FORTH_ERR_ARGS);

    ForthWord *top = AC_TOP_WORD(interp);

    // die if not string.
    if (top->type != WORD_STRING) {
        forth_die(interp, "forth_pop_str", FORTH_ERR_UNEXPECTED_TYPE);
    }
    if (top->strval.str == NULL) {
        if (top->tok_str.str == NULL) {
            forth_uneval_word(interp, top);
        }
        forth_eval_word(interp, top);
    }

    forth_pop_str_fast(interp, str);
}


// faster than forth_pop_word(). do not check the top word's type.
// NOTE: strval must be set.
void
forth_pop_str_fast(ForthInterp *interp, char *str)
{
    ForthWord *top = AC_TOP_WORD(interp);

    // assign
    strncpy(str, top->strval.str, strlen(top->strval.str) + 1);

    if (top->tok_str.str != NULL) {
        forth_debugf(interp, "pop![%s]\n", top->tok_str.str);
    } else {
        forth_debug(interp, "pop!\n");
    }
    // pop
    word_destruct(top);
    stack_pop(interp->word_stack);
}


// - convert tok_str to digit.
// - check the top word's type.
void
forth_pop_digit(ForthInterp *interp, digit_t *digit)
{
    if (digit == NULL)
        forth_die(interp, "forth_pop_digit", FORTH_ERR_ARGS);

    ForthWord *top = AC_TOP_WORD(interp);

    // die if not digit.
    if (top->type != WORD_DIGIT) {
        forth_die(interp, "forth_pop_digit", FORTH_ERR_UNEXPECTED_TYPE);
    }
    // convert if digit is not set.
    if (! top->digitval.is_set) {
        if (top->tok_str.str == NULL) {
            forth_uneval_word(interp, top);
        }
        forth_eval_word(interp, top);
    }

    forth_pop_digit_fast(interp, digit);
}


// faster than forth_pop_word(). do not check the top word's type.
// NOTE: digitval must be set.
void
forth_pop_digit_fast(ForthInterp *interp, digit_t *digit)
{
    ForthWord *top = AC_TOP_WORD(interp);

    // assign
    *digit = top->digitval.digit;

    if (top->tok_str.str != NULL) {
        forth_debugf(interp, "pop![%s]\n", top->tok_str.str);
    } else {
        forth_debug(interp, "pop!\n");
    }
    // pop
    word_destruct(top);
    stack_pop(interp->word_stack);
}

