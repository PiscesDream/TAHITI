#ifndef _SYSCALL_H_
#define _SYSCALL_H_

void init_syscall();

//
// format from linux      // arguments
#define __NR_null       0
#define __NR_null1      1 // 1
#define __NR_null2      2 // 1
#define __NR_null3      3 // 1
#define __NR_fork       4 // 0
#define __NR_share_fork 5 // 0
#define __NR_exit       6 // 1
#define __NR_wait       7 // 0
#define __NR_getpid     8 // 0
#define __NR_putch      9 // 1
#define __NR_ls         10 // 0
#define __NR_get_key    11 // 0
#define __NR_tree       12 // 0

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
type name(atype a,btype b) \
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



#endif
