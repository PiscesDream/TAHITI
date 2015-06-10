#ifndef _SYSCALL_H_
#define _SYSCALL_H_

// format from linux      // arguments
#define __NR_exec_cmd   0 // 1
#define __NR_null1      1 // 1
#define __NR_null2      2 // 1
#define __NR_puts       3 // 1
#define __NR_fork       4 // 0
#define __NR_null5      5 // 0
#define __NR_exit       6 // 1
#define __NR_wait       7 // 0
#define __NR_getpid     8 // 0
#define __NR_putch      9 // 1
#define __NR_ls         10 // 0
#define __NR_get_key    11 // 0
#define __NR_tree       12 // 0
#define __NR_sem_apply  13 // 1
#define __NR_sem_free   14 // 1
#define __NR_sem_wait   15 // 1
#define __NR_sem_signal 16 // 1
#define __NR_print_path 17 // 0
#define __NR_fopen      18 // 1
#define __NR_fclose     19 // 1
#define __NR_fputch     20 // 2
#define __NR_fgetch     21 // 1
#define __NR_fseek      22 // 2



#define _syscall0(type,name) \
  type syscall_##name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

#define _syscall1(type,name,atype,a) \
type syscall_##name(atype a) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a))); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

#define _syscall2(type,name,atype,a,btype,b) \
type syscall_##name(atype a,btype b) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b))); \
if (__res >= 0) \
	return (type) __res; \
return -1; \
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a,btype b,ctype c) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(a)),"c" ((long)(b)),"d" ((long)(c))); \
if (__res>=0) \
	return (type) __res; \
return -1; \
}

int syscall_fork();
int syscall_exit(int);
int syscall_wait();
int syscall_getpid();
int syscall_putch(char);
int syscall_ls();
int syscall_tree();

int syscall_sem_apply();
int syscall_sem_wait();
int syscall_sem_signal();
int syscall_sem_free();
int syscall_sem_free();

int syscall_puts(char *);
int syscall_print_path();

int syscall_fopen(char *);
int syscall_fclose(int file);
int syscall_fputch(int file, char c);
int syscall_fgetch(int file);
int syscall_fseek(int file, int pos);



#endif
