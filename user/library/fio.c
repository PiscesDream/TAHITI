#include <syscall.h>
#include <fio.h>
#include <stdio.h>

file_t fopen(char * s) {
    printf("    >>system: opening file [%s]\n", s);
    return syscall_fopen(s);
}

void fclose(file_t f) {
    syscall_fclose(f);
}

void fputch(file_t f, char c) {
    syscall_fputch(f, c);
}

char fgetch(file_t f) {
    return syscall_fgetch(f);
}

void fseek(file_t f, int pos) {
    syscall_fseek(f, pos);
}

