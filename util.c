/*
 * util.c - functions which does NOT take 'ForthInterp' as arg 1
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-25.
 *
 */
#include "util.h"

#include "preproc.h"


#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>



int
d_printf(const char* format, ...)
{
#if NDEBUG
    va_list ap;
    int result;

    va_start(ap, format);
    result = vprintf(format, ap);
    va_end(ap);

    return result;
#else
    UNUSED_ARG(format);
    return 1;
#endif
}

int
d_print(const char* msg)
{
#if NDEBUG
    for (; *msg != '\0'; msg++) fputc(*msg, stdout);
    return 1;
#else
    UNUSED_ARG(msg);
    return 1;
#endif
}


bool
is_word(int c)
{
    return ! isspace(c);
}


