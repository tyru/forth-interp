#ifndef WORD_H
#define WORD_H


#include "type.h"


typedef void (*forth_word_t)(ForthInterp *);

// forth operators
struct ForthWord {
    char            *name;
    forth_word_t    func;
};



void
forth_word_plus(ForthInterp *interp);

void
forth_word_minus(ForthInterp *interp);

void
forth_word_multiply(ForthInterp *interp);

void
forth_word_divide(ForthInterp *interp);


#endif /* WORD_H */
