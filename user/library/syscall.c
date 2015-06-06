#include <syscall.h>

_syscall0(int, fork);
_syscall1(int, exit, int, exit_code);
_syscall0(int, wait);
_syscall0(int, getpid);
_syscall1(int, putch, char, ch);
_syscall0(int, ls);
_syscall0(int, get_key);
_syscall0(int, tree);

