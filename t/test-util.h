#ifndef TEST_H
#define TEST_H


#include <stdio.h>
#include <stdlib.h>



#define OK(cond, msg) \
    ok(cond, msg, __FILE__, __LINE__)

void ok(int cond, const char* msg, const char *file, const int line)
{
    static int count;
    if (cond)
        printf("%d: %s: *ok*\n", count, msg);
    else {
        printf("%d: %s: *NOT ok*\n", count, msg);
        fprintf(stderr, "died: at file %s, line %d\n", file, line);
        exit(EXIT_FAILURE);
    }
    count++;
}


void die(char *msg)
{
    fprintf(stderr, "%s: failed.", msg);
    exit(EXIT_FAILURE);
}


#endif /* TEST_H */
