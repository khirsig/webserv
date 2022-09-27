#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    for (size_t i = 0; i < 10; i++) {
        if (i % 2) {
            continue;
        }
        printf("test %zu\n", i);
    }
}
