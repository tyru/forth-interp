/*
 * parser.c - parser
 *
 * Written By: tyru <tyru.exe@gmail.com>
 * Last Change: 2009-08-28.
 *
 */

#include "parser.h"

#include "util.h"
#include "forth.h"    // accessing to interp, must know full structure of ForthInterp.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



// NOTE:
// - max_size <= capacity of token->name
// - this free() token if not NULL.
bool
forth_get_token_from_src(
        ForthInterp *interp,
        ForthToken *token,
        size_t max_size)
{
    char token_name[max_size];    // c99

    if (interp->src == NULL) goto FORTH_ERR_ARGS;
    if (token == NULL) goto FORTH_ERR_ARGS;

    // free() previous token.
    FREE(token->name);


    // skip the non-word character.
    while (1) {
        if (interp->src[interp->cur_pos] == '\0')
            goto FORTH_ERR_EOF;
        if (! is_skip_char(interp->src[interp->cur_pos]))
            break;
        interp->cur_pos++;
    }

    // get token.
    for (; ; interp->cur_pos++) {
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
                    forth_die(interp, "forth_get_token_from_src", FORTH_ERR_UNCLOSED_STR);
                }
                /* found it */
                else if (*(end - 1) != '\\') {    // if not escaped string.
                    // copy string inside double quotes.
                    size_t inside_len = (end - 1) - (begin + 1);
                    if (inside_len > max_size) goto FORTH_ERR_OVERFLOW;
                    strncpy(token_name, begin + 1, inside_len);
                    token_name[inside_len] = '\0';

                    break;
                }
                /* found but it was escaped. search again. */
                else {
                    cur_srch_pos = end;
                }
            }

            if (! is_string(token_name)) {
                forth_die(interp, "forth_get_token_from_src", FORTH_ERR_BAD_STRING);
            }

            goto END_PARSING;
        }

        // digit
        if (isdigit(c)) {
            char *begin = interp->src + interp->cur_pos;
            char *end;

            while (interp->src[interp->cur_pos] != '\0'    // not null byte
            && (isdigit(interp->src[interp->cur_pos])    // is digit
            || interp->src[interp->cur_pos] == '.'))    // is dot
            {
                interp->cur_pos++;
            }
            end = interp->src + interp->cur_pos;

            // copy token to token_name.
            if (CAST(size_t, end - begin) > max_size) goto FORTH_ERR_OVERFLOW;
            strncpy(token_name, begin, end - begin);
            token_name[end - begin] = '\0';

            if (! is_digit(token_name)) {
                forth_die(interp, "forth_get_token_from_src", FORTH_ERR_BAD_DIGIT);
            }

            goto END_PARSING;
        }

        // or something word.
        if (! is_skip_char(c)) {
            char *begin = interp->src + interp->cur_pos;
            char *end;

            while (interp->src[interp->cur_pos] != '\0'    // not null byte
            && ! is_skip_char(interp->src[interp->cur_pos]))    // is not skip character
            {
                interp->cur_pos++;
            }
            end = interp->src + interp->cur_pos;

            // copy token to token_name.
            if (CAST(size_t, end - begin) > max_size) goto FORTH_ERR_OVERFLOW;
            strncpy(token_name, begin, end - begin);
            token_name[end - begin] = '\0';

            // no checking.

            goto END_PARSING;
        }


        // you would not reach this block...
        ASSERT(0);
    }

    size_t token_len;
END_PARSING:
    /* allocate and copy to token->name as needed bytes */
    token_len = strlen(token_name);
    token->name = forth_malloc_noret(interp, token_len + 1);
    strncpy(token->name, token_name, token_len + 1);

    // identify token type.
    token->type = forth_get_token_type(interp, token->name);

    // there was no error.
    interp->errno = FORTH_ERR_NOERR;

    return true;


    /* error handling */
FORTH_ERR_ARGS:
    forth_die(interp, "forth_get_token_from_src", FORTH_ERR_ARGS);
    /* NOTREACHED */

FORTH_ERR_EOF:
    interp->errno = FORTH_ERR_EOF;
    token->name = NULL;
    return false;

FORTH_ERR_OVERFLOW:
    interp->errno = FORTH_ERR_OVERFLOW;
    token->name = NULL;
    return false;
}
