#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../forth.h"
#include "../util.h"



static ForthInterp interp;
ForthInterp *i = &interp;

void
destruct(void)
{
    forth_destruct(&interp);
}


void
atod_test(void)
{
    char *failed;


    /* integer numbers */

    failed = NULL;
    OK((int)digit_t2double(atod("1", 10, &failed)) == 1, "\"1\" is 1");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("123", 10, &failed)) == 123, "\"123\" is 123");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK((int)digit_t2double(atod("-1", 10, &failed)) == -1, "\"-1\" is -1");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK((int)digit_t2double(atod("+1", 10, &failed)) == 1, "\"+1\" is 1");
    OK(failed == NULL, "not failed");


    /* float numbers */
    /* XXX should I add 'volatile' to somewhere? */

    failed = NULL;
    OK(digit_t2double(atod("1.1", 10, &failed)) == 1.1, "\"1.1\" is 1.1");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("+1.1", 10, &failed)) == 1.1, "\"+1.1\" is 1.1");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("-1.1", 10, &failed)) == -1.1, "\"-1.1\" is -1.1");
    OK(failed == NULL, "not failed");


    failed = NULL;
    OK(digit_t2double(atod("123.4", 10, &failed)) == 123.4, "\"123.4\" is 123.4");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("+123.4", 10, &failed)) == 123.4, "\"+123.4\" is 123.4");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("-123.4", 10, &failed)) == -123.4, "\"-123.4\" is -123.4");
    OK(failed == NULL, "not failed");


    failed = NULL;
    OK(digit_t2double(atod("1.234", 10, &failed)) == 1.234, "\"1.234\" is 1.234");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("+1.234", 10, &failed)) == 1.234, "\"+1.234\" is 1.234");
    OK(failed == NULL, "not failed");

    failed = NULL;
    OK(digit_t2double(atod("-1.234", 10, &failed)) == -1.234, "\"-1.234\" is -1.234");
    OK(failed == NULL, "not failed");


    /* these string must be failed to convert */

    failed = NULL;
    OK(digit_t2double(atod("abc", 10, &failed)) == 0, "\"abc\" is failed to convert");
    OK(failed != NULL, "failed!");

    failed = NULL;
    OK(digit_t2double(atod("123abc", 10, &failed)) == 0, "\"123abc\" is failed to convert");
    OK(failed != NULL, "failed!");

    failed = NULL;
    OK(digit_t2double(atod("abc123", 10, &failed)) == 0, "\"abc123\" is failed to convert");
    OK(failed != NULL, "failed!");

    failed = NULL;
    OK(digit_t2double(atod("abc123def", 10, &failed)) == 0, "\"abc123def\" is failed to convert");
    OK(failed != NULL, "failed!");

    failed = NULL;
    OK(digit_t2double(atod("123abc456", 10, &failed)) == 0, "\"123abc456\" is failed to convert");
    OK(failed != NULL, "failed!");
}


void
dtoa_test(void)
{
    const int max_size = 1024;
    char str[max_size];    // c99
    char *failed;


    memset(str, 0, max_size);
    OK(dtoa(1, str, max_size, 10), "conversion succeeded");
    OK(str[0] != '\0', "not empty string");

    failed = NULL;
    OK(digit_t2double(atod(str, 10, &failed)) == 1, "digit -> ascii -> digit: two digits are the same");
    OK(failed == NULL, "not failed");


    memset(str, 0, max_size);
    OK(dtoa(-1, str, max_size, 10), "conversion succeeded");
    OK(str[0] != '\0', "not empty string");

    failed = NULL;
    OK(digit_t2double(atod(str, 10, &failed)) == -1, "digit -> ascii -> digit: two digits are the same");
    OK(failed == NULL, "not failed");


    memset(str, 0, max_size);
    OK(dtoa(1.5, str, max_size, 10), "conversion succeeded");
    OK(str[0] != '\0', "not empty string");

    failed = NULL;
    OK(digit_t2double(atod(str, 10, &failed)) == 1.5, "digit -> ascii -> digit: two digits are the same");
    OK(failed == NULL, "not failed");
}


int
main(void)
{
    // construct.
    forth_init(i);

    atexit(destruct);


    atod_test();

    dtoa_test();


    return EXIT_SUCCESS;
}

