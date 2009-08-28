/*
 * util.c - functions which does NOT take 'ForthInterp' as arg 1
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include "util.h"

#include "word.h"
#include "token.h"

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
    fputs(msg, stdout);
    return 1;
#else
    UNUSED_ARG(msg);
    return 1;
#endif
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


// token_type -> word_type
word_type
token_type2word_type(token_type type)
{
    return (word_type)type;
}


// word_type -> token_type
token_type
word_type2token_id(word_type type)
{
    return (token_type)type;
}


// true: success
// false: can't allocate memory for word
bool
token_to_word_copy(ForthWord *word, ForthToken *token)
{
    word->name = CAST(char*, malloc(strlen(token->name) + 1));
    if (word->name == NULL) return false;

    word->type = token_type2word_type(token->type);
    word->func = CAST(word_func_t, 0);

    return true;
}
