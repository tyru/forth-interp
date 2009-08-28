#ifndef PARSER_H
#define PARSER_H

#include "type.h"

#include <stdio.h>
#include <stdbool.h>



// on success, return true.
// on failure, return false.
// (in case of that, use forth_{die,perror,exit}())
bool
forth_get_token_from_src(ForthInterp *interp, ForthToken *token, size_t max_size);



#endif /* PARSER_H */
