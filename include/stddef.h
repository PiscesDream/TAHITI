#ifndef _STDDEF_H_
#define _STDDEF_H_

typedef struct{
    unsigned int ss, gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, userss;   /* pushed by the processor automatically */ 
    // if the privilege doesn't change the useresp and userss is omitted
} exception_status_t;

#endif
