#include "server.h"

void hand_err(int res, const char *fn) {
    if (res >= 0) {
        return;
    }

    if (fn) {
        perror(fn);
        unlink(DEF_SERVER_PATH);
    }
    exit(1);
}

void hand_sigint(int sig) {
    if (sig != SIGINT) {
        return;
    }

    if (unlink(DEF_SERVER_PATH) == -1) {
        perror("unlink()");
    }
    exit(0);
}
