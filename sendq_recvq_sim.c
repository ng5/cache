#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 12345
#define MSG_SIZE 4096
#define SEND_COUNT 10000

void *server_thread(void *arg) {
    int srv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    bind(srv_fd, (struct sockaddr *) &addr, sizeof(addr));
    listen(srv_fd, 1);

    int cli_fd = accept(srv_fd, NULL, NULL);

    char buf[MSG_SIZE];
    for (int i = 0; i < SEND_COUNT; ++i) {
        // Slow read: sleep to simulate backlog
        usleep(5000); // 5ms
        read(cli_fd, buf, MSG_SIZE);
    }
    close(cli_fd);
    close(srv_fd);
    return NULL;
}

void *client_thread(void *arg) {
    sleep(1); // Ensure server is ready
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(PORT);

    connect(fd, (struct sockaddr *) &addr, sizeof(addr));

    char buf[MSG_SIZE];
    memset(buf, 'A', MSG_SIZE);
    for (int i = 0; i < SEND_COUNT; ++i) {
        write(fd, buf, MSG_SIZE);
        // No delay: send as fast as possible
    }
    close(fd);
    return NULL;
}

int main() {
    pthread_t srv, cli;
    pthread_create(&srv, NULL, server_thread, NULL);
    pthread_create(&cli, NULL, client_thread, NULL);

    pthread_join(cli, NULL);
    pthread_join(srv, NULL);

    printf("Done. Use 'ss -tanp' or 'netstat -tanp' during run to observe send-Q/recv-Q.\n");
    return 0;
}
