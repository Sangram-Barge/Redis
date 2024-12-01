#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void err_shutdown(const char *msg) {
    int error = errno;
    fprintf(stderr, "[%d] %s\n", error, msg);
    abort();
}

int main() {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        err_shutdown("socket() failed");
    }

    struct sockaddr_in addr = {};
}
     