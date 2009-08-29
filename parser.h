#ifndef PARSER_H
#define PARSER_H

#include "type.h"

#include <stdio.h>
#include <stdbool.h>


#define is_skip_char(c) \
    ( \
        ((c) != '\0') \
        && \
        isspace(c) \
    )

#define is_digit_char(c) \
    ( \
        ((c) != '\0') \
        && \
        (! is_skip_char(c)) \
        && \
        (isdigit(c) || (c) == '.') \
    )

#define is_word_char(c) \
    ( \
        ((c) != '\0') \
        && \
        (! is_skip_char(c)) \
    )



// on success, return true.
// on failure, return false.
// (in case of that, use forth_{die,perror,exit}())
bool
forth_get_token_from_src(ForthInterp *interp, char *token, size_t max_size);



#endif /* PARSER_H */
