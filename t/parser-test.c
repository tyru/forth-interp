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
    const int max_size = 1024;    // max length of source code in this test.
    char word[max_size];    // c99

    atexit(destruct);


    // construct.
    forth_init(i);


    /* set and check - begin */

    // "foo"
    forth_set_src(i, "foo");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "foo"), "word is \"foo\"");

    forth_get_word_from_src(i, word, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(*word == '\0', "no more words");


    // "foo bar"
    forth_set_src(i, "foo bar");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "foo"), "word is \"foo\"");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "bar"), "word is \"bar\"");

    forth_get_word_from_src(i, word, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(*word == '\0', "no more words");


    // "    foo bar"
    forth_set_src(i, "    foo bar");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "foo"), "word is \"foo\"");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "bar"), "word is \"bar\"");

    forth_get_word_from_src(i, word, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(*word == '\0', "no more words");


    // " \t\n\ffoo\nbar"
    forth_set_src(i, " \t\n\ffoo\nbar");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "foo"), "word is \"foo\"");

    forth_get_word_from_src(i, word, max_size);
    OK(STREQ(word, "bar"), "word is \"bar\"");

    forth_get_word_from_src(i, word, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(*word == '\0', "no more words");

    /* set and check - end */


    // destruct.
    forth_destruct(i);


    return EXIT_SUCCESS;
}

