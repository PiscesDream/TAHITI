#ifndef _FIO_H_
#define _FIO_H_

#include <fs/fs.h>
typedef struct __fbuffer{ 
    char * buffer;
    int ptr;
    file_t * file; 
} fbuffer_t;

#endif
