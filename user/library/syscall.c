#include <syscall.h>

_syscall0(int, fork);
_syscall1(int, exit, int, exit_code);
_syscall0(int, wait);
_syscall0(int, getpid);
_syscall1(int, putch, char, ch);
_syscall0(int, ls);
_syscall0(int, get_key);
_syscall0(int, tree);
_syscall1(int, sem_apply, int, value);
_syscall1(int, sem_free, int, num);
_syscall1(int, sem_wait, int, num);
_syscall1(int, sem_signal, int, num);


