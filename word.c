/*
 * word.c - forth operator definitions
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-30.
 *
 */

#include "word.h"

#include "util.h"
#include "forth.h"

#include <stdlib.h>
#include <errno.h>



/* api */

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


void
word_init_with_digit(ForthWord *word, digit_t digit)
{
    word_init(word);
    word->type = WORD_DIGIT;
    word_set_digit(word, digit);
}


void
word_init_with_tok_str(ForthWord *word, const char *tok_str)
{
    word_init(word);
    word_set_tok_str_copy(word, tok_str);
}


// NOT USED
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


void
word_set_tok_str(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->tok_str.str = CAST(char*, str);

    word->tok_str.capacity = -1;    // not allocated!
    word->tok_str.len = len;
}


void
word_set_tok_str_copy(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->tok_str.str = malloc(len + 1);
    if (word->tok_str.str == NULL) {
        FREE(word->tok_str.str);    // for safety
        return;
    }
    if (errno == ENOMEM) {
        FREE(word->tok_str.str);    // for safety
        return;
    }

    strcpy(word->tok_str.str, str);

    word->tok_str.capacity = len + 1;
    word->tok_str.len = len;
}


// NOT USED
void
word_set_str(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->strval.str = CAST(char*, str);
    word->strval.capacity = -1;    // not allocated!
    word->strval.len = len;
}


void
word_set_str_copy(ForthWord *word, const char *str)
{
    size_t len = strlen(str);

    word->strval.str = malloc(len + 1);
    if (word->strval.str == NULL) {
        FREE(word->strval.str);    // for safety
        return;
    }
    if (errno == ENOMEM) {
        FREE(word->strval.str);    // for safety
        return;
    }

    strcpy(word->strval.str, str);

    word->strval.capacity = len + 1;
    word->strval.len = len;
}


void
word_set_digit(ForthWord *word, digit_t digit)
{
    word->digitval.digit = digit;
    word->digitval.is_set = true;
}


/* forth operator definitions */

void
forth_word_plus(ForthInterp *interp)
{
    // assertions
    ASSERT(interp, AC_TOP_WORD(interp)->type == WORD_DIGIT);
    ASSERT(interp, AC_TOP_WORD(interp)->digitval.is_set);
    // pop
    digit_t n = AC_TOP_WORD(interp)->digitval.digit;
    stack_pop(&(interp->word_stack));

    // assertions
    ASSERT(interp, AC_TOP_WORD(interp)->type == WORD_DIGIT);
    ASSERT(interp, AC_TOP_WORD(interp)->digitval.is_set);
    // pop
    digit_t m = AC_TOP_WORD(interp)->digitval.digit;
    stack_pop(&(interp->word_stack));


    // this is most important thing!
    digit_t result = n + m;


    ForthWord word;
    word_init_with_digit(&word, result);

    // push
    stack_push(&(interp->word_stack), &word);
}


void
forth_word_minus(ForthInterp *interp)
{
    // TODO
    UNUSED_ARG(interp);
}


void
forth_word_multiply(ForthInterp *interp)
{
    // TODO
    UNUSED_ARG(interp);
}


void
forth_word_divide(ForthInterp *interp)
{
    // TODO
    UNUSED_ARG(interp);
}
