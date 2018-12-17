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

#define MAX_HISTORY 100
int shell_screen[MAX_HISTORY][2];
int shell_history[MAX_HISTORY][2];

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

struct task_info task1 = {"task1", (uint32_t)&ready_to_exit_task, USER_PROCESS};
struct task_info task2 = {"task2", (uint32_t)&wait_lock_task, USER_PROCESS};
struct task_info task3 = {"task3", (uint32_t)&wait_exit_task, USER_PROCESS};

struct task_info task4 = {"task4", (uint32_t)&semaphore_add_task1, USER_PROCESS};
struct task_info task5 = {"task5", (uint32_t)&semaphore_add_task2, USER_PROCESS};
struct task_info task6 = {"task6", (uint32_t)&semaphore_add_task3, USER_PROCESS};

struct task_info task7 = {"task7", (uint32_t)&producer_task, USER_PROCESS};
struct task_info task8 = {"task8", (uint32_t)&consumer_task1, USER_PROCESS};
struct task_info task9 = {"task9", (uint32_t)&consumer_task2, USER_PROCESS};

struct task_info task10 = {"task10", (uint32_t)&barrier_task1, USER_PROCESS};
struct task_info task11 = {"task11", (uint32_t)&barrier_task2, USER_PROCESS};
struct task_info task12 = {"task12", (uint32_t)&barrier_task3, USER_PROCESS};

struct task_info task13 = {"SunQuan",(uint32_t)&SunQuan, USER_PROCESS};
struct task_info task14 = {"LiuBei", (uint32_t)&LiuBei, USER_PROCESS};
struct task_info task15 = {"CaoCao", (uint32_t)&CaoCao, USER_PROCESS};

struct task_info *test_tasks[16] = {&task1, &task2, &task3,
                                           &task4, &task5, &task6,
                                           &task7, &task8, &task9,
                                           &task10, &task11, &task12,
                                           &task13, &task14, &task15};
static int num_test_tasks = 15;



void test_shell()
{
    char content[10][20];
    int current = 0;
    sys_move_cursor(1, 15);
    printf("-------------------------    UCAS_OS    --------------------------\n");
    printf("> root@UCAS_OS:");
    int i=0;
    int row=0;
    int index=0;
    int screen_number=0;
    while (1)
    {
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

        // TODO solve command
    }
}