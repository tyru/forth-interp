#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"

#include "../util.h"
#include "../util.h"



int main(void)
{
    OK(strcount("foo", 'f') == 1, "\"foo\" contains one 'f'");
    OK(strcount("foo", 'o') == 2, "\"foo\" contains two 'o'");
    OK(strcount("foo", '\n') == 0, "\"foo\" contains no newlines");
    OK(strcount("foo\nbar\nbaz", '\n') == 2, "\"foo\\nbar\\nbaz\" contains two newlines");


    return EXIT_SUCCESS;
}

