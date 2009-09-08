#ifndef WORD_H
#define WORD_H


#include "type.h"
#include "digit.h"

#include <stdlib.h>
#include <stdbool.h>


#define WORD_NULL_FUNC      ((word_func_t)0)

#define WORD_FUNC_STR       "(function)"


#define POP_CONV_DIGIT(interp, dval) \
    do { \
        if (AC_TOP_WORD(interp)->tok_str.str == NULL) { \
            forth_uneval_word(interp, AC_TOP_WORD(interp)); \
        } \
        forth_eval_word(interp, AC_TOP_WORD(interp)); \
        \
        ASSERT(interp, AC_TOP_WORD(interp)->type == WORD_DIGIT); \
        ASSERT(interp, AC_TOP_WORD(interp)->digitval.is_set); \
        \
        dval = AC_TOP_WORD(interp)->digitval.digit; \
        \
        d_printf("pop![%s]\n", AC_TOP_WORD(interp)->tok_str.str); \
        word_destruct(AC_TOP_WORD(interp)); \
        stack_pop(interp->word_stack); \
    } while (0)

#define POP_CONV_SOME(interp, args, args_len, type) \
    do { \
        if (interp->word_stack->cur_pos + 1 < args_len) { \
            interp->errid = FORTH_ERR_TOO_FEW_ARGS; \
            return; \
        } \
        for (int i = args_len - 1; i >= 0; i--) { \
            POP_CONV_ ## type(interp, args[i]); \
        } \
    } while (0)



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



/* word api */

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


/* forth word api */

void
forth_init_word(ForthInterp *interp);

// NOTE: copy the address of tok_str.
void
forth_regist_word(ForthInterp *interp, const char *tok_str, word_func_t func);

char*
forth_word_as_str(ForthInterp *interp, ForthWord *word);

// evaluate word->tok_str.
// NOTE: word->type and word->tok_str must be set.
void
forth_eval_word(ForthInterp *interp, ForthWord *word);

void
forth_uneval_word(ForthInterp *interp, ForthWord *word);

word_type
forth_get_word_type(ForthInterp *interp, const char *token);

ForthWord*
forth_get_word_def(ForthInterp *interp, const char *token);


/* word functions */

void
forth_word_plus(ForthInterp *interp);

void
forth_word_minus(ForthInterp *interp);

void
forth_word_multiply(ForthInterp *interp);

void
forth_word_divide(ForthInterp *interp);


#endif /* WORD_H */
