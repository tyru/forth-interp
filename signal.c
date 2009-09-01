#include "signal.h"

#include "util.h"
#include "forth.h"

#include <string.h>
#include <signal.h>


// register signal handlers.
void
forth_regist_sighandler(ForthInterp *interp, void (*handler)(int))
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));

    // destruct_interp() traps all signals.
    sa.sa_handler = handler;
    sa.sa_flags |= SA_RESTART;

    const int signals[] = {
#ifdef SIGABRT
        SIGABRT,
#endif
#ifdef SIGALRM
        SIGALRM,
#endif
#ifdef SIGALRM
        SIGALRM,
#endif
#ifdef SIGBUS
        SIGBUS,
#endif
#ifdef SIGFPE
        SIGFPE,
#endif
#ifdef SIGHUP
        SIGHUP,
#endif
#ifdef SIGILL
        SIGILL,
#endif
#ifdef SIGINT
        SIGINT,
#endif
#ifdef SIGIO
        SIGIO,
#endif
#ifdef SIGPIPE
        SIGPIPE,
#endif
#ifdef SIGPWR
        SIGPWR,
#endif
#ifdef SIGQUIT
        SIGQUIT,
#endif
#ifdef SIGSEGV
        SIGSEGV,
#endif
#ifdef SIGSTKFLT
        SIGSTKFLT,
#endif
#ifdef SIGSYS
        SIGSYS,
#endif
#ifdef SIGTERM
        SIGTERM,
#endif
#ifdef SIGSYS
        SIGSYS,
#endif
#ifdef SIGSYS
        SIGSYS,
#endif
#ifdef SIGVTALRM
        SIGVTALRM,
#endif
#ifdef SIGXCPU
        SIGXCPU,
#endif
#ifdef SIGXFSZ
        SIGXFSZ,
#endif
    };
    const int len = sizeof(signals) / sizeof(int);
    d_printf("len is %d\n", len);

    for (int i = 0; i < len; i++) {
        if (sigaction(signals[i], &sa, NULL) != 0) {
            // failed to register
            d_printf("failed to register signal: %d: ", signals[i]);
            forth_die(interp, "sigaction", FORTH_ERR_INIT);
        }
    }
}
