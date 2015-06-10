#include <fs/fio.h>
#include <fs/fs.h>
#include <stddef.h>
#include <utils/system.h>
#include <task.h>
#include <screen.h>
#include <heap.h>
#include <fs/hd.h>


#define FBUFFER_MAX 10
fbuffer_t fbuf[FBUFFER_MAX] = {0}; 

int get_empty_fbuf() {
    int i = 0;
    for (i = 0; i < FBUFFER_MAX; ++i)
        if (fbuf[i].file == 0) return i;
    return -1;
}

int fopen_for_syscall(exception_status_t * t) {
    int i = get_empty_fbuf();
    if (i == -1) 
        panic("All file buffers are in used !");

    char * filename = (char *)(cur_task->base + t->ebx);
    file_t * tmp = find_file(filename);
    if (tmp == 0) {
        kprintf("File %S is not found", filename);
        panic("!");
    }

    fbuf[i].file = tmp;    
    fbuf[i].ptr = 0;
    uint32_t cluster_count;
    fbuf[i].buffer = (char *)read_hd_fat(tmp->cluster, &cluster_count);

    return i;
}

int fclose_for_syscall(exception_status_t * t) {
    int i = t->ebx;
    if (fbuf[i].file == 0)
        panic("File hasn't been opened!");

    write_hd_fat(fbuf[i].file->cluster, fbuf[i].buffer);
    fbuf[i].file = 0;
}

int fputch_for_syscall(exception_status_t * t) {
    int i = t->ebx;
    if (fbuf[i].file == 0)
        panic("File hasn't been opened!");

    fbuf[i].buffer[ fbuf[i].ptr++ ] = (char)t->ecx;

    if (fbuf[i].ptr > fbuf[i].file->length)
        panic("Out of file!");
}

int fgetch_for_syscall(exception_status_t * t) {
    int i = t->ebx;
    if (fbuf[i].file == 0)
        panic("File hasn't been opened!");

    return fbuf[i].buffer[ fbuf[i].ptr++ ]; 
}

int fseek_for_syscall(exception_status_t * t) {
    int i = t->ebx;
    if (fbuf[i].file == 0)
        panic("File hasn't been opened!");

    if (t->ecx > fbuf[i].file->length)
        panic("Out of file!");

    fbuf[i].ptr = t->ecx;
}
