#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

#define NUM_MESSAGES 10000000
#define MAX_JSON_LEN 64 // conservative upper bound

int main() {
    constexpr auto total_size = NUM_MESSAGES * MAX_JSON_LEN;

    // Allocate anonymous memory with mmap
    auto *buf =
            static_cast<char *>(mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    if (buf == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Fill buffer with formatted JSON
    char *p = buf;
    for (int i = 0; i < NUM_MESSAGES; i++) {
        constexpr auto JSON_TEMPLATE = R"({"id":%d,"name":"Alice","active":true})";
        const auto len = snprintf(p, MAX_JSON_LEN, JSON_TEMPLATE, i);
        p += len;
    }

    const size_t bytes_written = p - buf;

    // Write entire buffer in one syscall
    write(STDOUT_FILENO, buf, bytes_written);

    // Clean up
    munmap(buf, total_size);
    return 0;
}
