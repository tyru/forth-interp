#ifndef SIGNAL_H
#define SIGNAL_H

#include "type.h"

// register signal handlers.
void
forth_regist_sighandler(ForthInterp *interp, void (*handler)(int));

const char*
sig2str(int signo);


#endif /* SIGNAL_H */
