// This program is a test of dll.

#include <stdio.h>

extern int add(int a, int b);

int main() {
    printf("Hello, World!\n");
    printf("1+2=%d\n", add(1, 2));
    return 0;
}