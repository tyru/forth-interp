#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../forth.h"



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

    atexit(destruct);

    // construct.
    forth_init(i);

    // forth_get_token_type()
    OK(forth_get_token_type(i, "+") == WORD_FUNC);
    OK(forth_get_token_type(i, "-") == WORD_FUNC);
    OK(forth_get_token_type(i, "*") == WORD_FUNC);
    OK(forth_get_token_type(i, "/") == WORD_FUNC);

    // forth_get_token_func()
    OK(forth_get_token_func(i, "+") == forth_word_plus);
    OK(forth_get_token_func(i, "-") == forth_word_minus);
    OK(forth_get_token_func(i, "*") == forth_word_multiply);
    OK(forth_get_token_func(i, "/") == forth_word_divide);


    return EXIT_SUCCESS;
}

