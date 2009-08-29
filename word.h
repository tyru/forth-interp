#ifndef WORD_H
#define WORD_H


#include "type.h"
#include "digit.h"

#include <stdlib.h>
#include <stdbool.h>


#define WORD_NULL_FUNC      ((word_func_t)0)

#define WORD_FUNC_STR       "(function)"



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
    // string value.
    struct word_str_t {
        char *str;
        int capacity;
        int len;
    } strval;
    // digit value.
    struct word_digit_t {
        digit_t digit;
        bool is_set;
    } digitval;
    // parsed string.
    struct word_tok_str_t {
        char *str;
        int capacity;
        int len;
    } tok_str;

    word_func_t     func;
    word_type       type;
};

typedef struct word_value_t word_value_t;



/* api */

void
word_init(ForthWord *word);

void
word_init_with_digit(ForthWord *word, digit_t digit);

void
word_init_with_tok_str(ForthWord *word, const char *tok_str);

void
word_init_with_str(ForthWord *word, const char *str);

void
word_destruct(ForthWord *word);

void
word_set_tok_str(ForthWord *word, const char *str);

void
word_set_tok_str_copy(ForthWord *word, const char *str);

void
word_set_str(ForthWord *word, const char *str);

void
word_set_str_copy(ForthWord *word, const char *str);

void
word_set_digit(ForthWord *word, digit_t digit);


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
