/*
 * util.c - functions which does NOT take 'ForthInterp' as arg 1
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-27.
 *
 */

#include "util.h"

#include "preproc.h"


#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>



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
    return
        ! isspace(c)
        ;
}


// count c in s.
size_t
strcount(const char *s, int c)
{
    char *found;
    size_t count = 0;
    while ((found = strchr(s, c)) != NULL) {
        count++;
        if (*(found + 1) == '\0') return count;
        s = found + 1;
    }
    return count;
}


// from http://d.hatena.ne.jp/p95095yy/20070205/1170688704
char*
strtok_r(char *str, const char *delim, char **saveptr)
{
    size_t width;
    char   *head;

    head = (NULL != str) ? str : *saveptr;
    width = strcspn(head, delim);
    if (0 == width) {
        head += strspn(head, delim);
        width = strcspn(head, delim);
    }
    if ('\0' == *head) {
        return NULL;
    }

    *saveptr = head + width + strspn(head + width, delim);
    *(head + width) = '\0';

    return head;
}


