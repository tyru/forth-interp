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

    atexit(destruct);

    // construct.
    forth_init(i);


    /* forth_get_word_type() */
    OK(forth_get_word_type(i, "+") == WORD_FUNC, "+ is func word");
    OK(forth_get_word_type(i, "-") == WORD_FUNC, "- is func word");
    OK(forth_get_word_type(i, "*") == WORD_FUNC, "* is func word");
    OK(forth_get_word_type(i, "/") == WORD_FUNC, "/ is func word");


    /* forth_get_word_def() */
    ForthWord *def;

    OK((def = forth_get_word_def(i, "+")) != NULL, "exist word func of +");
    OK(def->func == forth_word_plus, "func name is forth_word_plus");

    OK((def = forth_get_word_def(i, "-")) != NULL, "exist word func of -");
    OK(def->func == forth_word_minus, "func name is forth_word_minus");

    OK((def = forth_get_word_def(i, "*")) != NULL, "exist word func of *");
    OK(def->func == forth_word_multiply, "func name is forth_word_multiply");

    OK((def = forth_get_word_def(i, "/")) != NULL, "exist word func of /");
    OK(def->func == forth_word_divide, "func name is forth_word_divide");


    return EXIT_SUCCESS;
}

