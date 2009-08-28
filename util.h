#ifndef UTIL_H
#define UTIL_H

#include "token.h"
#include "word.h"

#include <stdlib.h>    // for size_t
#include <string.h>    // for STREQ()
#include <stdbool.h>    // for bool



/* macro */

#define UNUSED_ARG(x)               ((void)x)

#define DEREF(type, ptr)            (*(type*)(ptr))
#define CAST(type, val)             ((type)(val))

#define STREQ(s1, s2) \
    (*(s1) == *(s2) && strcmp((s1), (s2)) == 0)

// NOTE: FREE() makes code spam-ish.
#define FREE(ptr) \
    do { \
        if (ptr != NULL) { \
            free(ptr); \
            ptr = NULL; \
        } \
    } while (0)

#define ASSERT(cond) \
    do { \
        if (! cond) { \
            forth_die(interp, "ASSERT", FORTH_ERR_ASSERT_FAILED); \
        } \
    } while (0)



/* util functions */

int
d_printf(const char* format, ...);

int
d_print(const char* msg);

// count c in s.
size_t
strcount(const char *s, int c);

// from http://d.hatena.ne.jp/p95095yy/20070205/1170688704
char*
strtok_r(char *str, const char *delim, char **saveptr);

// token_type -> word_type
word_type
token_type2word_type(token_type type);

// word_type -> token_type
token_type
word_type2token_id(word_type type);

// true: success
// false: can't allocate memory for word
bool
token_to_word(ForthWord *word, ForthToken *token);


#endif /* UTIL_H */
