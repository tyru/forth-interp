#ifndef TOKEN_H
#define TOKEN_H

#include "type.h"

#include <stdbool.h>



// string is closed with double quotes.
bool
is_string(const char *token);

bool
is_digit(const char *token);


#endif /* TOKEN_H */
