/*
 * main.c - main
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-01.
 *
 */

#include "forth.h"
#include "signal.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>



static volatile ForthInterp *gl_interp;




void
destruct_interp(int signo)
{
    psignal(signo, NULL);
    if (gl_interp != NULL)
        forth_die(CAST(ForthInterp*, gl_interp), NULL, FORTH_ERR_SIG);
}


int
main(int argc, char **argv)
{
    ForthInterp interp;
    gl_interp = &interp;

    // construct.
    forth_init(&interp);
    // register signal handlers.
    forth_regist_sighandler(&interp, destruct_interp);
    // parsing arguments.
    forth_getopt(&interp, &argc, argv);
    // read and execute each line.
    forth_repl(&interp);
    // destruct.
    forth_destruct(&interp);
    gl_interp = NULL;

    return EXIT_SUCCESS;
}

