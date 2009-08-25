#ifndef TEST_H
#define TEST_H


#include <stdio.h>
#include <stdlib.h>


void ok(int cond, char* msg)
{
    static int count;
    if (cond)
        printf("%d: %s: *ok*\n", count, msg);
    else {
        printf("%d: %s: *NOT ok*\n", count, msg);
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
