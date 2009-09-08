/*
 * util.c - functions which does NOT take 'ForthInterp' as arg 1
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-09.
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
#include <errno.h>



int
d_printf(const char* format, ...)
{
#if NDEBUG
    va_list ap;
    int result;

    fputs("[debug]::", stderr);

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
    fputs("[debug]::", stderr);
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


// almost code from 'man 3 strtol'.
// if failed, *failed is not NULL.
digit_t
atod(const char *digit_str, int base, char **failed)
{
    char *end_ptr;
    double val;

    // TODO support base
    UNUSED_ARG(base);

    errno = 0;
    val = strtod(digit_str, &end_ptr);

    if (errno == ERANGE || (errno != 0 && val == 0)) {
        *failed = CAST(char*, digit_str);
        return (digit_t)0;
    }
    if (end_ptr == digit_str) {
        *failed = CAST(char*, digit_str);
        return (digit_t)0;
    }
    if (*end_ptr != '\0') {
        *failed = CAST(char*, digit_str);
        return (digit_t)0;
    }

    return double2digit_t(val);
}


// on success, return true.
bool
dtoa(digit_t digit, char *ascii, size_t max_size, int base)
{
    UNUSED_ARG(base);    // TODO support base

    snprintf(ascii, max_size, "%f", digit);
    return true;
}


// convert token to word.
void
forth_token2word(ForthInterp *interp, const char *token, ForthWord *word)
{
    // identify token's type.
    word->type = forth_get_word_type(interp, token);
    d_printf("word type: %d\n", word->type);

    // copy parsed string to tok_str.
    word_set_tok_str_copy(word, token);

    // find token's func.
    if (word->type == WORD_FUNC) {
        ForthWord *w = forth_get_word_def(interp, token);
        if (w == NULL) return;
        word->func = w->func;
    }
    else {
        word->func = WORD_NULL_FUNC;
    }
}
