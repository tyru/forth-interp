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
forth_parse_args(int *argc, char **argv);

void
forth_repl(ForthInterp *interp);

char*
forth_fgets(ForthInterp *interp, FILE *in);

void
forth_exec_code(ForthInterp *interp);

bool
forth_code_eof(ForthInterp *interp);

bool
is_word(int c);



#endif /* FORTH_H */
