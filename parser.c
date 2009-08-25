#include "parser.h"

#include "util.h"

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
        if (interp->src[interp->cur_pos] == '\0') goto ERR_EOF;

        word[word_count] = interp->src[interp->cur_pos];
        word_count++;
        interp->cur_pos++;

        if (interp->cur_pos >= max_size) return true;    // this is success.
    }
    word[word_count] = '\0';
    return true;

    // set interp->errno.
ERR_ARGS:
    interp->errno = FORTH_ERR_ARGS;
    return false;
ERR_EOF:
    interp->errno = FORTH_ERR_EOF;
    return false;
}
