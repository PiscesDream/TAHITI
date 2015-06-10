#include <program_header.h>
#include <stdio.h>

int main() {
    int i = 0;
    printf("I am lab 11.\n");
    for (;;++i) {
        printf("Lab 11 is counting %d ...\n", i);
        if (i == 30000) i = 0;
    }
}
