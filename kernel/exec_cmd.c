#include <stddef.h>
#include <syscall.h>
#include <task.h>
#include <string.h>
#include <screen.h>
#include <stdbool.h>

#include <fs/fs.h>

extern void ls();

char *get_next_argv(char *s) {
    while (*s == ' ') s++;
    return s;
}

int exec_cmd_for_syscall(exception_status_t *t) {
    char * cmd = (char *)(cur_task->base + t->ebx);
    while (*cmd == ' ') cmd++;

    if (strncmp(cmd, "ls", 2)) 
        ls();
    else if (strncmp(cmd, "tree", 4))
        tree();
    else if (strncmp(cmd, "cd", 2)) {
        char * arg0 = get_next_argv(cmd+2);
        cd(arg0);
    }
    else if (strncmp(cmd, "cat", 3)) {
        char * arg0 = get_next_argv(cmd+3);
        cat(arg0);
    }
    else if (strncmp(cmd, "exec", 4)) {
        char * arg0 = get_next_argv(cmd+4);
        exec(arg0);
    }

}
