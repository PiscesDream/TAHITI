#include <program_header.h>
#include <stdio.h>
#include <fio.h>

int main() {
    printf("I am Lab 10\n");
    file_t file = fopen("HI"); 

    printf("file index: %d\n", file);
    int i = 0;
    for (i = 0; i<4; ++i)
        printf("%dth char is [%c]\n", i, fgetch(file));

    printf("seek to 10\n");
    fseek(file, 10);

    for (i = 0; i<4; ++i) {
        printf("put char [%c] to %dth char\n", i+'1', i+10);
        fputch(file, i+'1');
    }

    fclose(file); 
}
