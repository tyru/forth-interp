/*
 * main.c - main
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-26.
 *
 */

#include "forth.h"

#include <stdio.h>
#include <stdlib.h>




int
main(int argc, char **argv)
{
    ForthInterp interp;

    // construct.
    forth_init(&interp);
    // parsing arguments.
    forth_getopt(&interp, &argc, argv);
    // read and execute each line.
    forth_repl(&interp);
    // destruct.
    forth_destruct(&interp);

    return EXIT_SUCCESS;
}

