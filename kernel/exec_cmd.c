#include <stddef.h>
#include <syscall.h>
#include <task.h>
#include <string.h>
#include <screen.h>
#include <stdbool.h>

#include <fs/fs.h>

extern void ls();
void help();

char *get_next_argv(char *s) {
    while (*s != ' ' && s) s++;
    while (*s == ' ' && *s) s++;
    return s;
}

int exec_cmd_for_syscall(exception_status_t *t) {
    char * cmd = (char *)(cur_task->base + t->ebx);
    while (*cmd == ' ') cmd++;

    while (*cmd) {
        if (strncmp(cmd, "ls", 2)) 
            ls();
        else if (strncmp(cmd, "tree", 4))
            tree();
        else if (strncmp(cmd, "cd", 2)) {
            char * arg0 = get_next_argv(cmd);
            cd(arg0);
        }
        else if (strncmp(cmd, "cat", 3)) {
            char * arg0 = get_next_argv(cmd);
            cat(arg0);
        }
        else if (strncmp(cmd, "exec", 4)) {
            char * arg0 = get_next_argv(cmd);
            exec(arg0);
        }
        else if (strncmp(cmd, "help", 4)) {
            help();
        }
        else if (strncmp(cmd, "rm", 2)) {
            char * arg0 = get_next_argv(cmd);
            rm(arg0);
        }
        else if (strncmp(cmd, "cp", 2)) {
            char * arg0 = get_next_argv(cmd);
            char * arg1 = get_next_argv(arg0);
            cp(arg0, arg1);
        }
        else {
            printf("Command %S not found.\n", cmd);
        }

        while (*cmd != ',' && *cmd) cmd++;
    }
}


void help() {
    kprintf("ls                 display the file under current directory\n"
            "tree               traverse current directory\n"
            "help               display this information\n"
            "exec <filename>    execute a file\n"
            "cat <filename>     display a file\n"
            "cd <dirname>       change current directory to <dirname>\n" 
            );
}
