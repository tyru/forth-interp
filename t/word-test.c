#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../forth.h"
#include "../parser.h"
#include "../preproc.h"

static ForthInterp interp;

void
destruct(void)
{
    forth_destruct(&interp);
}


int
main(void)
{
    ForthInterp *i = &interp;
    const int max_size = 1024;
    char word[max_size];    // c99

    atexit(destruct);


    // construct.
    forth_init(i);


    /* set and check - begin */
    forth_set_src(i, "foo");
    forth_get_word_from_src(i, word, max_size);
    ok(STREQ(word, "foo"), "word is \"foo\"");

    forth_set_src(i, "foo bar");
    forth_get_word_from_src(i, word, max_size);
    ok(STREQ(word, "foo"), "word is \"foo\"");

    forth_set_src(i, "   foo bar");
    forth_get_word_from_src(i, word, max_size);
    ok(STREQ(word, "foo"), "word is \"foo\"");
    /* set and check - end */


    // destruct.
    forth_destruct(i);


    return EXIT_SUCCESS;
}

