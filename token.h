#ifndef TOKEN_H
#define TOKEN_H

#include "type.h"

#include <stdbool.h>



// not on c99 environment, this declaration may be invalid.
// (if next variable declaration was found)
#define DECL_TOKEN(varname) \
    ForthToken varname; \
    varname.name = NULL



// forth token type
enum token_type {
    TOKEN_UNDEF = 0,    // unknown token
    TOKEN_STRING,       // string
    TOKEN_DIGIT,        // digit
    TOKEN_FUNC,         // forth's word
};
typedef enum token_type token_type;


// forth token
struct ForthToken {
    char        *name;
    token_type  type;
};



void
token_destruct(ForthToken *token);

bool
is_skip_char(int c);

bool
is_string(const char *token);

bool
is_digit(const char *token);


#endif /* TOKEN_H */
