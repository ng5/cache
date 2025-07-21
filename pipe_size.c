#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef F_GETPIPE_SZ
#define F_GETPIPE_SZ 1032
#endif

#ifndef F_SETPIPE_SZ
#define F_SETPIPE_SZ 1031
#endif

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    int default_sz = fcntl(pipefd[0], F_GETPIPE_SZ);
    printf("Default pipe size: %d bytes\n", default_sz);

    return 0;
}

