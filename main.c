/*
 * main.c - 
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-25.
 *
 */

#include "forth.h"
// for ForthInterp. (in fact, forth.h includes this header though)
#include "struct.h"

#include <stdio.h>
#include <stdlib.h>




int main(int argc, char **argv)
{
    ForthInterp interp;

    // construct.
    forth_init(&interp);
    // parsing arguments.
    forth_parse_args(&argc, argv);
    // read and execute each line.
    forth_repl(&interp);
    // destruct.
    forth_destruct(&interp);

    return EXIT_SUCCESS;
}

