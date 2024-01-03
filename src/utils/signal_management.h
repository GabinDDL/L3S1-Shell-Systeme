#ifndef SIGNAL_MANAGEMENT_H
#define SIGNAL_MANAGEMENT_H

void use_jsh_signal_management();
/* jsh ignores SIGINT, SIGTERM, SIGTTIN, SIGQUIT,
SIGTTOU and SIGTSTP signals */

void reset_signal_management();
/* Reset SIGINT, SIGTERM, SIGTTIN, SIGQUIT,
SIGTTOU and SIGTSTP signals, which were previously ignored */

#endif
