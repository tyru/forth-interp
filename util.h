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
        if ((ptr) != NULL) { \
            free(ptr); \
            (ptr) = NULL; \
        } \
    } while (0)

#define ASSERT(interp, cond) \
    do { \
        if (! (cond)) { \
            fprintf(stderr, "file %s, line %d: ", __FILE__, __LINE__); \
            forth_die((interp), "ASSERT", FORTH_ERR_ASSERT_FAILED); \
        } \
    } while (0)

#define AC_TOP_WORD(interp) \
    CAST(ForthWord*, (interp)->word_stack.top)



/* util functions */

int
d_printf(const char* format, ...);

int
d_print(const char* msg);

// count c in s.
size_t
strcount(const char *s, int c);

// almost code from 'man 3 strtol'.
// if failed, *failed is not NULL.
digit_t
atod(const char *digit_str, int base, char **failed);

// on success, return true.
bool
dtoa(digit_t digit, char *ascii, size_t max_size, int base);


#endif /* UTIL_H */
