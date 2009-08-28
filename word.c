/*
 * word.c - forth operator definitions
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include "word.h"

#include "forth.h"




void
word_destruct(ForthWord *word)
{
    FREE(word->name);
}


/* forth operator definitions */

void
forth_word_plus(ForthInterp *interp)
{
}


void
forth_word_minus(ForthInterp *interp)
{
}


void
forth_word_multiply(ForthInterp *interp)
{
}


void
forth_word_divide(ForthInterp *interp)
{
}
