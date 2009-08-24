#ifndef PREPROC_H
#define PREPROC_H



/* define */
#define NDEBUG 1

/* constant */
#define SOURCE_MAX_LINEBYTE     (1024)
#define SOURCE_MAX_STRLEN       (100 * 1024)
#define REPL_PROMPT_STR         "> "

/* macro */
#define STREQ(s1, s2)               (*(s1) == *(s2) && strcmp((s1), (s2)) == 0)
#define UNUSED_ARG(x)               ((void)x)
#define DEREF(ptr, type)            (*(type*)ptr)
#define REINTERP_CAST(type, val)    ((type)val)



#endif /* PREPROC_H */
