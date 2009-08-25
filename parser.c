/*
 * parser.c - parser
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-26.
 *
 */

#include "parser.h"

#include "util.h"    // for is_word()
#include "forth.h"    // accessing to interp, must know full structure of ForthInterp.

#include <stdio.h>


bool
forth_get_word_from_src(
        ForthInterp *interp,
        char *word,
        size_t max_size)
{
    int word_count;

    if (interp->src == NULL) goto ERR_ARGS;
    if (word == NULL) goto ERR_ARGS;

    // skip the non-word character.
    while (1) {
        if (interp->src[interp->cur_pos] == '\0') goto ERR_EOF;
        if (is_word(interp->src[interp->cur_pos])) break;
        interp->cur_pos++;
    }

    // get word.
    word_count = 0;
    while (is_word(interp->src[interp->cur_pos])) {
        if (interp->src[interp->cur_pos] == '\0') break;    // this is success

        word[word_count] = interp->src[interp->cur_pos];
        word_count++;
        interp->cur_pos++;

        if (interp->cur_pos >= max_size) break;    // this is success.
    }
    word[word_count] = '\0';
    return true;

    // set interp->errno.
ERR_ARGS:
    interp->errno = FORTH_ERR_ARGS;
    return false;
ERR_EOF:
    interp->errno = FORTH_ERR_EOF;
    *word = '\0';    // make word empty string.
    return false;
}
