#ifndef UTIL_H
#define UTIL_H


#include <stdlib.h>    // for size_t
#include <stdbool.h>    // for bool


int
d_printf(const char* format, ...);

int
d_print(const char* msg);

bool
is_word(int c);

// count c in s.
size_t
strcount(const char *s, int c);

// from http://d.hatena.ne.jp/p95095yy/20070205/1170688704
char*
strtok_r(char *str, const char *delim, char **saveptr);


#endif /* UTIL_H */
