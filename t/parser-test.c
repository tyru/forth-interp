#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../forth.h"
#include "../parser.h"
#include "../util.h"
#include "../token.h"

#include <alloca.h>



static ForthInterp interp;
static ForthToken  token;

void
destruct(void)
{
    token_destruct(&token);
    forth_destruct(&interp);
}


int
main(void)
{
    ForthInterp *i = &interp;
    const int max_size = SRC_DFL_WORDBYTE;    // max length of source code in this test.


    atexit(destruct);


    // construct.
    token.name = NULL;
    forth_init(i);


    /* set and check - begin */

    // "foo"
    forth_set_src(i, "foo");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "foo"), "token.name is \"foo\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(token.name == NULL, "no more words");


    // "foo bar"
    forth_set_src(i, "foo bar");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "foo"), "token.name is \"foo\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "bar"), "token.name is \"bar\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(token.name == NULL, "no more words");


    // "    foo bar"
    forth_set_src(i, "    foo bar");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "foo"), "token.name is \"foo\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "bar"), "token.name is \"bar\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(token.name == NULL, "no more words");


    // " \t\n\ffoo\nbar"
    forth_set_src(i, " \t\n\ffoo\nbar");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "foo"), "token.name is \"foo\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(STREQ(token.name, "bar"), "token.name is \"bar\"");

    forth_get_token_from_src(i, &token, max_size);
    OK(i->errno == FORTH_ERR_EOF, "no more words");
    OK(token.name == NULL, "no more words");

    /* set and check - end */


    return EXIT_SUCCESS;
}

