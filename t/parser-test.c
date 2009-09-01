#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../forth.h"
#include "../parser.h"
#include "../util.h"
#include "../token.h"
#include "../constant.h"

#include <alloca.h>



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
    const int max_size = SRC_DFL_WORDBYTE;    // max length of source code in this test.
    char token[max_size];


    atexit(destruct);


    // construct.
    forth_init(i);


    /* set and check - begin */

    // "foo"
    forth_set_src(i, "foo");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "foo"), "token.name is \"foo\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(i->errid == FORTH_ERR_EOF, "no more words");
    OK(*token == '\0', "no more words");


    // "foo bar"
    forth_set_src(i, "foo bar");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "foo"), "token.name is \"foo\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "bar"), "token.name is \"bar\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(i->errid == FORTH_ERR_EOF, "no more words");
    OK(*token == '\0', "no more words");


    // "    foo bar"
    forth_set_src(i, "    foo bar");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "foo"), "token.name is \"foo\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "bar"), "token.name is \"bar\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(i->errid == FORTH_ERR_EOF, "no more words");
    OK(*token == '\0', "no more words");


    // " \t\n\ffoo\nbar"
    forth_set_src(i, " \t\n\ffoo\nbar");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "foo"), "token.name is \"foo\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(STREQ(token, "bar"), "token.name is \"bar\"");

    memset(token, 0, max_size);
    forth_get_token_from_src(i, token, max_size);
    OK(i->errid == FORTH_ERR_EOF, "no more words");
    OK(*token == '\0', "no more words");

    /* set and check - end */


    return EXIT_SUCCESS;
}

