#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE (4 * 1024 * 1024)  // 4 MB

int main() {
    char *buf = malloc(BUF_SIZE);
    if (!buf) return 1;

    while (read(STDIN_FILENO, buf, BUF_SIZE) > 0) {}
    free(buf);
    return 0;
}
