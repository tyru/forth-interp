/*
 * token.c - forth's tokens
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-30.
 *
 */

#include "token.h"
#include "util.h"

#include <ctype.h>
#include <string.h>




// string is closed with double quotes.
bool
is_string(const char *token)
{
    if (token[0] != '"') return false;

    size_t len = strlen(token);
    if (len < 2) return false;    // double quotes
    if (token[len - 1] != '"') return false;

    return true;
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


