#ifndef WORD_H
#define WORD_H


#include "type.h"



#define WORD_NULL_FUNC      ((word_func_t)NULL)

// not on c99 environment, this declaration may be invalid.
// (if next variable declaration was found)
#define DECL_WORD(varname) \
    ForthWord varname; \
    varname.func = WORD_NULL_FUNC; \
    varname.name = NULL



// forth's word
enum word_type {
    WORD_UNDEF = 0,    // unknown token
    WORD_STRING,       // string
    WORD_DIGIT,        // digit
    WORD_FUNC,         // forth's word
};
typedef enum word_type word_type;


// without volatile, comparison result may be wrong.
typedef void (*word_func_t)(ForthInterp *);


// forth operators
struct ForthWord {
    char            *name;
    word_type       type;
    word_func_t     func;
};




/* api */

void
word_destruct(ForthWord *word);


/* forth operator definitions */

void
forth_word_plus(ForthInterp *interp);

void
forth_word_minus(ForthInterp *interp);

void
forth_word_multiply(ForthInterp *interp);

void
forth_word_divide(ForthInterp *interp);


#endif /* WORD_H */
