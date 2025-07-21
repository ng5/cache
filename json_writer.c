#include <fcntl.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
    constexpr size_t buf_size = 1lu << 32lu; // 256KiB
    char *buf = malloc(buf_size);
    memset(buf, 'X', buf_size); // output Xs
    while (true) {
        size_t remaining = buf_size;
        while (remaining > 0) {
            const ssize_t written = write(STDOUT_FILENO, buf + (buf_size - remaining), remaining);
            remaining -= written;
        }
    }
}
