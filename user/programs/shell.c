#include <program_header.h>
#include <system.h>
#include <stdio.h>

char cmd[128];
int cmd_len;
int cmd_counter = 0;

int main()
{
    cmd_counter = 0;
    cmd_len = 0;
    while (1)
    {
        printf("\r\nIN [%d]: ", cmd_counter);

        while (1)
        {
            char key = getch();
            if (key=='\b')
            {
                if (cmd_len == 0) continue;
                type_char('\b');
                type_char(' ');
                type_char('\b');
                cmd_len--;
            }
            else if (key=='\r')
            {
                new_line();
                cmd[cmd_len] = '\0';
                run_cmd(cmd, files, 0);  // let the handler decide
                cmd_len = 0;
                break;
            }
            else
            {
                if (cmd_len >= CMD_MAX) continue;
                cmd[cmd_len++] = key;
                type_char(key);
            }
        }

        cmd_counter++;
    }
}

