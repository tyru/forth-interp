#ifndef PREPROC_H
#define PREPROC_H


#include <string.h>    // for STREQ


/* constant */

// default value of max length
#define SRC_DFL_BYTE         (100 * 1024)
#define SRC_DFL_LINEBYTE     (1024)
#define SRC_DFL_WORDBYTE     (1024)
#define STACK_DFL_NUM        (1024)

// prompt string of repl
#define REPL_PROMPT_STR         "> "


/* macro */
#define STREQ(s1, s2)               (*(s1) == *(s2) && strcmp((s1), (s2)) == 0)
#define UNUSED_ARG(x)               ((void)x)
#define DEREF(ptr, type)            (*(type*)ptr)
#define REINTERP_CAST(type, val)    ((type)val)



#endif /* PREPROC_H */
