/*
 * forth.c - api of forth
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-25.
 *
 */

#include "forth.h"

#include "struct.h"
#include "preproc.h"
#include "util.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#define _GNU_SOURCE
#include <getopt.h>


/* api */

void
forth_init(ForthInterp *interp)
{
    interp->code = REINTERP_CAST(char *, malloc(SOURCE_MAX_STRLEN));
    memset(interp->code, 0, SOURCE_MAX_STRLEN);

    interp->code_read_pos = 0;
}


void
forth_destruct(ForthInterp *interp)
{
    free(interp->code);
}


void
forth_parse_args(int *argc, char **argv)
{
    static struct option long_opts[] = {
        {"help", 0, 0, 'h'}
    };
    int opt_index = 0;
    int c;

    while ((c = getopt_long(*argc, argv, "h", long_opts, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                puts("May the forth be with you.");
                exit(EXIT_SUCCESS);

            case '?':
                perror("getopt_long");
                break;
        }
    }
}


void
forth_repl(ForthInterp *interp)
{
    char linebuf[SOURCE_MAX_LINEBYTE];

    while (1) {
        printf(REPL_PROMPT_STR);

        // clear buffer each line.
        memset(linebuf, 0, SOURCE_MAX_LINEBYTE);
        if (fgets(linebuf, SOURCE_MAX_LINEBYTE, stdin) == NULL)
            break;

        // set linebuf as source code.
        strncpy(interp->code, linebuf, SOURCE_MAX_STRLEN);
        // execute.
        forth_exec_code(interp);
    }
}


// NOTE: not used.
char*
forth_fgets(ForthInterp *interp, FILE *in)
{
    char linebuf[SOURCE_MAX_LINEBYTE];
    // get line and concat if not null.
    char *ret = fgets(linebuf, SOURCE_MAX_LINEBYTE, in);
    if (ret != NULL) {
        strncat(interp->code, linebuf, SOURCE_MAX_STRLEN);
    }
    // advance code_len.
    interp->code_len += strlen(linebuf);

    return ret;
}


void
forth_exec_code(ForthInterp *interp)
{
    d_printf("debug: [%s]\n", interp->code);
}

bool
forth_code_eof(ForthInterp *interp)
{
    return interp->code_read_pos > strlen(interp->code);
}
