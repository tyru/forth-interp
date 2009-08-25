#ifndef WORD_H
#define WORD_H


#include "type.h"


typedef void (*forth_word_func)(ForthInterp *interp);

// forth operators
struct ForthWord {
    char            *name;
    forth_word_func func;
};



void
forth_word_assign(ForthInterp *interp);


#endif /* WORD_H */
