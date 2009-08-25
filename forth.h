#ifndef FORTH_H
#define FORTH_H

#include "struct.h"

#include <stdio.h>
#include <stdbool.h>



/* api */


void
forth_init(ForthInterp *interp);

void
forth_destruct(ForthInterp *interp);

void
forth_getopt(ForthInterp *interp, int *argc, char **argv);

void
forth_repl(ForthInterp *interp);

void
forth_set_src(ForthInterp *interp, char *src);

void
forth_clear_src(ForthInterp *interp);

void
forth_exec_src(ForthInterp *interp);

void*
forth_malloc(ForthInterp *interp, size_t size);

void
forth_die(ForthInterp *interp, const char *msg, int status);

void
forth_perror(ForthInterp *interp, const char *msg);

void
forth_exit(ForthInterp *interp, int status);

bool
forth_src_eof(ForthInterp *interp);


#endif /* FORTH_H */
