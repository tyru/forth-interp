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




    return EXIT_SUCCESS;
}

