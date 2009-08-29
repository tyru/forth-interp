#ifndef TOKEN_H
#define TOKEN_H

#include "type.h"

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



bool
is_string(const char *token);

bool
is_digit(const char *token);


#endif /* TOKEN_H */
