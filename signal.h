#ifndef SIGNAL_H
#define SIGNAL_H

#include "type.h"

// register signal handlers.
void
forth_regist_sighandler(ForthInterp *interp, void (*handler)(int));


#endif /* SIGNAL_H */
