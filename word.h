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
    struct word_tokstr_t {
        char *str;
        int capacity;
        int len;
    } tokstr;

    word_func_t     func;
    word_type       type;
};

typedef struct word_value_t word_value_t;



/* word api */

void
word_init(ForthWord *word);

void
word_init_with_digit(ForthWord *word, digit_t digit);

void
word_init_with_tokstr(ForthWord *word, const char *tokstr);

void
word_init_with_str(ForthWord *word, const char *str);

void
word_destruct(ForthWord *word);

void
word_set_tokstr(ForthWord *word, const char *str);

void
word_set_tokstr_copy(ForthWord *word, const char *str);

void
word_set_str(ForthWord *word, const char *str);

void
word_set_str_copy(ForthWord *word, const char *str);

void
word_set_digit(ForthWord *word, digit_t digit);


/* forth word api */

void
forth_init_word(ForthInterp *interp);

// NOTE: copy the address of tokstr.
void
forth_regist_word(ForthInterp *interp, const char *tokstr, word_func_t func);

char*
forth_word_as_tokstr(ForthInterp *interp, ForthWord *word);

// evaluate word->tokstr.
// NOTE: word->type and word->tokstr must be set.
void
forth_eval_word(ForthInterp *interp, ForthWord *word);

void
forth_uneval_word(ForthInterp *interp, ForthWord *word);

word_type
forth_get_word_type(ForthInterp *interp, const char *token);

ForthWord*
forth_get_word_def(ForthInterp *interp, const char *token);


/* utility functions for word functions */

bool
forth_pop_word(ForthInterp *interp, ForthWord *word);

// faster than forth_pop_word(), and check the top word's type.
bool
forth_pop_str(ForthInterp *interp, char *str);

// faster than forth_pop_word(). do not check the top word's type.
// NOTE: strval must be set.
bool
forth_pop_str_fast(ForthInterp *interp, char *str);

// faster than forth_pop_word(), and check the top word's type.
bool
forth_pop_digit(ForthInterp *interp, digit_t *digit);

// faster than forth_pop_word(). do not check the top word's type.
// NOTE: digitval must be set.
bool
forth_pop_digit_fast(ForthInterp *interp, digit_t *digit);


#endif /* WORD_H */
