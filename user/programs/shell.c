#include <program_header.h>
#include <system.h>
#include <stdio.h>

#define CMD_MAX 128
char cmd[128];
int cmd_len;
int cmd_counter = 0;

char * buildin[] = {"ls", "cd", "tree", "cat", "exec"};
//int buildin_count = 5; 

extern int syscall_exec_cmd();

int main()
{
    cmd_counter = 0;
    cmd_len = 0;
    while (1)
    {
        printf("ROOT@TAHITI:");
        print_path();
        printf("$ ");

        while (1)
        {
            char key = getch();
            if (key=='\b')
            {
                if (cmd_len == 0) continue;
                putch('\b');
                putch(' ');
                putch('\b');
                cmd_len--;
            }
            else if (key=='\n')
            {
                putch('\n');
                cmd[cmd_len] = '\0';

                syscall_exec_cmd(cmd); // let the handler decide

                cmd_len = 0;
                break;
            }
            else
            {
                if (cmd_len >= CMD_MAX) continue;
                cmd[cmd_len++] = key;
                putch(key);
            }
        }

        cmd_counter++;
    }
}



