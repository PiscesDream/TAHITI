#include <syscall.h>

_syscall0(int, fork);
_syscall1(int, exit, int, exit_code);
_syscall0(int, wait);
_syscall0(int, getpid);
_syscall1(int, putch, char, ch);
//_syscall0(int, ls);
_syscall0(int, get_key);
//_syscall0(int, tree);
_syscall1(int, sem_apply, int, value);
_syscall1(int, sem_free, int, num);
_syscall1(int, sem_wait, int, num);
_syscall1(int, sem_signal, int, num);
//_syscall1(int, puts, char *, s);
_syscall1(int, exec_cmd, char *, cmd);
_syscall0(int, print_path);

_syscall1(int, fopen, char *, filename);
_syscall1(int, fclose, int, file);
_syscall2(int, fputch, int, file, char, c);
_syscall1(int, fgetch, int, file);
_syscall2(int, fseek, int, file, int, pos);



