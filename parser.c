/*
 * parser.c - parser
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-09-09.
 *
 */

#include "parser.h"

#include "util.h"
#include "forth.h"    // accessing to interp, must know full structure of ForthInterp.
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



// NOTE:
// - max_size <= capacity of token->tok_str
// - this free() token if not NULL.
// XXX
// check null byte.
// TODO
// fallback to other types.
bool
forth_get_token_from_src(
        ForthInterp *interp,
        char *token,
        size_t max_size)
{
    bool found = false;

    if (token == NULL) goto FORTH_ERR_ARGS;
    if (interp->src == NULL) goto FORTH_ERR_ARGS;


    // skip the non-word character.
    while (1) {
        if (interp->src[interp->cur_pos] == '\0')
            goto FORTH_ERR_EOF;
        if (! is_skip_char(interp->src[interp->cur_pos]))
            break;
        interp->cur_pos++;
    }

    // get token.
    for (; interp->cur_pos < interp->src_len; interp->cur_pos++) {
        int c = interp->src[interp->cur_pos];

        if (is_skip_char(c))
            continue;

        // string
        if (c == '"') {
            char *begin = interp->src + interp->cur_pos;
            char *cur_srch_pos = begin + 1;    // position to begin to search.
            char *end;    // end of string.

            while (1) {
                end = strchr(cur_srch_pos, '"');

                /* not found '"' */
                if (end == NULL) {
                    interp->errid = FORTH_ERR_UNCLOSED_STR;
                    return false;
                }
                /* found it */
                else if (*(end - 1) != '\\') {    // if not escaped string.
                    size_t len = end - begin + 1;

                    ASSERT(interp, begin < end);
                    if (CAST(size_t, len) > max_size)
                        goto FORTH_ERR_OVERFLOW;

                    // copy string with double quotes.
                    strncpy(token, begin, len);
                    token[len] = '\0';

                    interp->cur_pos += len;

                    break;
                }
                /* found but it was escaped. search again. */
                else {
                    cur_srch_pos = end;
                }
            }

            if (! is_string(token)) {
                fprintf(stderr, "%s: ", token);
                interp->errid = FORTH_ERR_BAD_STRING;
                return false;
            }

            forth_debugf(interp, "string token found: %s\n", token);
            found = true;

            goto END_PARSING;
        }

        // digit
        if (isdigit(c)) {
            char *begin = interp->src + interp->cur_pos;
            char *end;

            while (is_digit_char(interp->src[interp->cur_pos])) {
                if (interp->cur_pos >= interp->src_len)
                    goto END_PARSING;
                else if (interp->src[interp->cur_pos] == '\0')
                    goto END_PARSING;
                else
                    interp->cur_pos++;
            }
            end = interp->src + interp->cur_pos;

            ASSERT(interp, begin < end);
            if (CAST(size_t, end - begin) > max_size)
                goto FORTH_ERR_OVERFLOW;

            // copy token to token.
            strncpy(token, begin, end - begin);
            token[end - begin] = '\0';

            if (! is_digit(token)) {
                fprintf(stderr, "%s: ", token);
                interp->errid = FORTH_ERR_BAD_DIGIT;
                return false;
            }

            forth_debugf(interp, "digit token found: %s\n", token);
            found = true;
            goto END_PARSING;
        }

        // or something word.
        if (! is_skip_char(c)) {
            char *begin = interp->src + interp->cur_pos;
            char *end;

            while (is_word_char(interp->src[interp->cur_pos])) {
                if (interp->cur_pos >= interp->src_len)
                    goto END_PARSING;
                else if (interp->src[interp->cur_pos] == '\0')
                    goto END_PARSING;
                else
                    interp->cur_pos++;
            }
            end = interp->src + interp->cur_pos;

            ASSERT(interp, begin < end);
            if (CAST(size_t, end - begin) > max_size)
                goto FORTH_ERR_OVERFLOW;

            // copy token to token.
            strncpy(token, begin, end - begin);
            token[end - begin] = '\0';

            // no checking.

            forth_debugf(interp, "undefined token found: %s\n", token);
            found = true;
            goto END_PARSING;
        }


        // you would not reach this block...
        ASSERT(interp, 0);
    }


END_PARSING:
    if (found) {
        // there was no error.
        interp->errid = FORTH_ERR_NOERR;
        return true;
    }
    else {
        interp->errid = FORTH_ERR_NOT_FOUND_TOKEN;
        return false;
    }


FORTH_ERR_EOF:
    interp->errid = FORTH_ERR_EOF;
    token[0] = '\0';
    return false;

    /* fatal error handling */

FORTH_ERR_ARGS:
    forth_die(interp, "forth_get_token_from_src", FORTH_ERR_ARGS);
    /* NOTREACHED */

    // gcc won't read above!
    return false;

FORTH_ERR_OVERFLOW:
    forth_die(interp, "forth_get_token_from_src", FORTH_ERR_OVERFLOW);
    /* NOTREACHED */

    return false;
}


// get token, convert it, push it to interp->word.
bool
forth_get_word(ForthInterp *interp)
{
    char token[interp->max_word_len];    // c99
    ForthWord word;

    word_init(&word);

    // parse interp->src
    // NOTE: token will be allocated.
    bool success = forth_get_token_from_src(interp, token, interp->max_word_len);

    if (success) {
        forth_debugf(interp, "read [%s].\n", token);
        // convert: char* -> ForthWord
        forth_token2word(interp, token, &word);
        // push it.
        forth_debugf(interp, "push![%s]\n", token);
        stack_push(interp->word_stack, &word);
    }

    // don't free pushed word! stack_push() copies only all bytes of struct.
    // word_destruct(&word);

    // to trap EOF outside of this function,
    // this func must return this value.
    // (do not die in this func!)
    return success;

    // 'word' at stack area is free()d but copied to interp->word_stack.
}
