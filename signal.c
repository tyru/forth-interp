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
#ifdef SIGTERM
        SIGTERM,
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

    for (int i = 0; i < len; i++) {
        if (sigaction(signals[i], &sa, NULL) != 0) {
            // failed to register
            d_printf("failed to register signal: %d: ", signals[i]);
            forth_die(interp, "sigaction", FORTH_ERR_INIT);
        }
    }
}


const char*
sig2str(int signo)
{

#define RET2STR(sym) case sym: return #sym;

    switch (signo) {
#ifdef SIGABRT
        RET2STR(SIGABRT);
#endif
#ifdef SIGALRM
        RET2STR(SIGALRM);
#endif
#ifdef SIGBUS
        RET2STR(SIGBUS);
#endif
#ifdef SIGFPE
        RET2STR(SIGFPE);
#endif
#ifdef SIGHUP
        RET2STR(SIGHUP);
#endif
#ifdef SIGILL
        RET2STR(SIGILL);
#endif
#ifdef SIGINT
        RET2STR(SIGINT);
#endif
#ifdef SIGIO
        RET2STR(SIGIO);
#endif
#ifdef SIGPIPE
        RET2STR(SIGPIPE);
#endif
#ifdef SIGPWR
        RET2STR(SIGPWR);
#endif
#ifdef SIGQUIT
        RET2STR(SIGQUIT);
#endif
#ifdef SIGSEGV
        RET2STR(SIGSEGV);
#endif
#ifdef SIGSTKFLT
        RET2STR(SIGSTKFLT);
#endif
#ifdef SIGTERM
        RET2STR(SIGTERM);
#endif
#ifdef SIGSYS
        RET2STR(SIGSYS);
#endif
#ifdef SIGVTALRM
        RET2STR(SIGVTALRM);
#endif
#ifdef SIGXCPU
        RET2STR(SIGXCPU);
#endif
#ifdef SIGXFSZ
        RET2STR(SIGXFSZ);
#endif
        default:
            return "undefined signal";
    }

#undef SYM2STR
}
