/*
 * token.c - forth's tokens
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include "token.h"
#include "util.h"

#include <ctype.h>
#include <string.h>




void
token_destruct(ForthToken *token)
{
    FREE(token->name);
}


bool
is_skip_char(int c)
{
    return
        isspace(c)
        ;
}


bool
is_string(const char *token)
{
    return
        token[0] == '"' &&
        token[strlen(token)] == '"'
        ;
}


bool
is_digit(const char *token)
{
    size_t dotnum = strcount(token, '.');

    if (token[0] == '.')
        return false;
    if (! (dotnum == 0 || dotnum == 1))
        return false;

    while (*token != '\0') {
        if (! (isdigit(*token) || *token == '.'))
            return false;
        token++;
    }

    return true;
}


