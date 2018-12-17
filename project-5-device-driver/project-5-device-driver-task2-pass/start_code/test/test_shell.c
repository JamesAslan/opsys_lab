/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *                  The shell acts as a task running in user mode. 
 *       The main function is to make system calls through the user's output.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software 
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * */

#include "test.h"
#include "stdio.h"
#include "screen.h"
#include "syscall.h"
#include "time.h"

#define MAX_HISTORY 10
int shell_screen[MAX_HISTORY][2];
int shell_history[MAX_HISTORY][2];


// pid = 2
// test exit
void test_shell_task1()
{
    int i;

    for (i = 0; i < 500; i++)
    {
        sys_move_cursor(0, 0);
        printf("I am Task A.(%d)           \n", i);
    }

    sys_exit();
}

// pid = 3
// test kill & waitpid
void test_shell_task2()
{
    int i;

    sys_move_cursor(0, 1);
    printf("I am waiting Task A to exit.\n");
    sys_waitpid(2);

    for (i = 0;; i++)
    {
        sys_move_cursor(0, 1);
        printf("I am Task B.(%d)           \n", i);
    }
}

// pid = 4
// test waitpid
void test_shell_task3()
{
    int i;

    sys_move_cursor(0, 2);
    printf("I am waiting Task B to exit.\n");
    sys_waitpid(3);

    for (i = 0;; i++)
    {
        sys_move_cursor(0, 2);
        printf("I am Task C.(%d)           \n", i);
    }
}

static void disable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status &= 0xfffffffe;
    set_cp0_status(cp0_status);
}

static void enable_interrupt()
{
    uint32_t cp0_status = get_cp0_status();
    cp0_status |= 0x01;
    set_cp0_status(cp0_status);
}

#define SHLLE_BUFF_SIZE 64

static int shell_tail = 0;
static char shell_buff[SHLLE_BUFF_SIZE];

static char read_uart_ch(void)
{
    char ch = 0;
    unsigned char *read_port = (unsigned char *)(0xbfe48000 + 0x00);
    unsigned char *stat_port = (unsigned char *)(0xbfe48000 + 0x05);

    while ((*stat_port & 0x01))
    {
        ch = *read_port;
    }
    return ch;
}

#if 1
struct task_info task4_1 = {"send",(uint32_t)&phy_regs_task1, USER_PROCESS};
struct task_info task4_2 = {"recv",(uint32_t)&phy_regs_task2, USER_PROCESS};
struct task_info task4_3 = {"initmac",(uint32_t)&phy_regs_task3, USER_PROCESS};


static struct task_info *test_tasks[19] = {&task4_3,&task4_1,&task4_2};
static int num_test_tasks = 3;
#endif




void test_shell()
{
    char content[10][20];
    int current = 0;
    sys_move_cursor(50, 29);
    printf("running time:%d",time_elapsed);
    sys_move_cursor(1, 15);
    printf("-------------------------    UCAS_OS    --------------------------\n");
    printf("> root@UCAS_OS:");
    int i=0;
    int row=0;
    int index=0;
    int screen_number=0;
    int screen_x = screen_cursor_x;
    int screen_y = screen_cursor_y;
    while (1)
    {
        sys_move_cursor(screen_x, screen_y);
        // read command from UART port
        disable_interrupt();
        char ch = read_uart_ch();
        enable_interrupt();
        if (ch == 0)
        {
            continue;
        }
        else if (ch == 127)
        {
            if(screen_number >0)
            {
                screen_number--;
                screen_cursor_x = shell_screen[screen_number][0];
                screen_cursor_y = shell_screen[screen_number][1];
                printf(" ");
                content[shell_history[screen_number][0]][shell_history[screen_number][1]]='\0';
                row=shell_history[screen_number][0];
                current=shell_history[screen_number][1];
                screen_cursor_x = shell_screen[screen_number][0];
                screen_cursor_y = shell_screen[screen_number][1];
            }
        }
        else if (ch == 13)
        {
            if(index ==1)
                content[row][current]='\0';
            if(strcmp(content[0],"ps")== 0)
            {
                int input_arg=0;
                if(strcmp(content[1],"a") == 0);
                    input_arg =1;
                sys_move_cursor(0, ++screen_cursor_y);
                sys_ps(input_arg);
                    //sys_move_cursor(0, screen_cursor_y);
                printf("> root@UCAS_OS:");
            }
            else if(strcmp(content[0],"clear")== 0)
            {
                    sys_clear(0,29);
                    sys_move_cursor(1, 15);
                    printf("-------------------------    UCAS_OS    --------------------------\n");
                    printf("> root@UCAS_OS:");
            }
            else if(strcmp(content[0],"exec")== 0)
            {
                sys_move_cursor(0, ++screen_cursor_y);
                int i;
                //if(strcmp(content[1],"0")==0)
                //{
                    int target =0;
                    target = content[1][0] - '0';
                    if(content[1][1] != '\0')
                        target = target*10 + content[1][1] - '0';
                    sys_spawn(test_tasks[target]);
                    //printf("exec process[%d]\n",target);
                    printf("> root@UCAS_OS:");
            }
            else if(strcmp(content[0],"kill")== 0)
            {
                int i;
                int target = content[1][0] - '0';
                sys_kill(target);
                sys_move_cursor(0, ++screen_cursor_y);
                printf("> root@UCAS_OS:");
            }
            else{
                //printf("%s\n",content[0]);
                sys_move_cursor(0, ++screen_cursor_y);
                printf("> root@UCAS_OS:");
            }
            for(i=0;i<10;i++)
            {
                content[i][0]='\0';
            }
            current =0;
            row =0;
            index =0;
            screen_number=0;
        }
        else
        {
            if(screen_number < MAX_HISTORY)
            {
                shell_screen[screen_number][0]=screen_cursor_x;
                shell_screen[screen_number][1]=screen_cursor_y;
                shell_history[screen_number][0]=row;
                shell_history[screen_number][1]=current;
                screen_number++;
            }
            if (ch == ' ')
            {
                printf("%c",ch);
                if(index == 0)
                {
                    continue;
                }
                else
                {
                    content[row][current]='\0';
                    index=0;
                    row++;
                    current=0;
                }
            }
            else
            {
                index = 1;
                printf("%c",ch);
                content[row][current++] = ch;
            }
        }
        screen_x = screen_cursor_x;
        screen_y = screen_cursor_y;
        sys_move_cursor(50, 29);
        printf("running time:%d",time_elapsed);
        // TODO solve command
    }
}