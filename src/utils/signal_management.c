#include "signal_management.h"
#include <assert.h>
#include <signal.h>
#include <stdlib.h>

void use_jsh_signal_management() {
    struct sigaction sigac_ignore;
    sigac_ignore.sa_handler = SIG_IGN;
    sigac_ignore.sa_flags = 0;

    assert(sigemptyset(&sigac_ignore.sa_mask) >= 0);

    assert(sigaction(SIGINT, &sigac_ignore, NULL) >= 0);
    assert(sigaction(SIGTERM, &sigac_ignore, NULL) >= 0);
    assert(sigaction(SIGTTIN, &sigac_ignore, NULL) >= 0);
    assert(sigaction(SIGQUIT, &sigac_ignore, NULL) >= 0);
    assert(sigaction(SIGTTOU, &sigac_ignore, NULL) >= 0);
    assert(sigaction(SIGTSTP, &sigac_ignore, NULL) >= 0);
}

void reset_signal_management() {
    struct sigaction sigac_reset;
    sigac_reset.sa_handler = SIG_DFL;
    sigac_reset.sa_flags = 0;

    assert(sigemptyset(&sigac_reset.sa_mask) >= 0);

    assert(sigaction(SIGINT, &sigac_reset, NULL) >= 0);
    assert(sigaction(SIGTERM, &sigac_reset, NULL) >= 0);
    assert(sigaction(SIGTTIN, &sigac_reset, NULL) >= 0);
    assert(sigaction(SIGQUIT, &sigac_reset, NULL) >= 0);
    assert(sigaction(SIGTTOU, &sigac_reset, NULL) >= 0);
    assert(sigaction(SIGTSTP, &sigac_reset, NULL) >= 0);
}
