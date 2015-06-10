#ifndef _FIO_H_
#define _FIO_H_

typedef int file_t;
file_t fopen(char * s) ;
void fclose(file_t f) ;
void fputch(file_t f, char c) ;
char fgetch(file_t f) ;
void fseek(file_t f, int pos) ;

#endif
