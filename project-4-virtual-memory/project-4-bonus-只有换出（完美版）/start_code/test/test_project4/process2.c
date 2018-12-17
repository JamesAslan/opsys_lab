#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"
#include "screen.h"
#include "test4.h"
#include "test.h"

#define RW_TIMES 2

int address = 0x20000;

int rand()
{	
	int current_time = get_timer();
	return current_time % 100000;
}

/*static void disable_interrupt()
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
*/
static void scanf(int *mem)
{
	//TODO:Use read_uart_ch() to complete scanf(), read input as a hex number.
	//Extending function parameters to (const char *fmt, ...) as printf is recommended but not required.
	char ch=0;
	int result=0;
	disable_interrupt();
	int cur_y=screen_cursor_y;
	screen_move_cursor(1,cur_y);
	while (1)
	{
    	char ch = read_uart_ch();
		if(ch <='f' && ch >='a')
			result = result*16 + ch - 'a' + 10;
		else if (ch <='9' && ch >='0')
			result  = result*16 + ch - '0';
		else if(ch != 0 && ch!=13)
		{
			//screen_move_cursor(0,2);
			my_printk("wrong!");
		}
		my_printk("%c",ch);
		if(ch == 13)
		{
			break;
		}		
	}
	//printk("3333333333333333333");
	*mem = result;
	//my_printk("%x",result);
	enable_interrupt();
}

void rw_task1(void)
{
	int mem1, mem2 = 0;
	int curs = 1;
	int memory[RW_TIMES];
	int i = 0;
	for(i = 0; i < RW_TIMES; i++)
	{
		sys_move_cursor(1, curs+i);
		scanf(&mem1);
		sys_move_cursor(1, curs+i);
		memory[i] = mem2 = rand();
		*(int *)mem1 = mem2;
		printf("Write: 0x%x, %d\n", mem1, mem2);
	}
	curs = RW_TIMES;
	for(i = 0; i < RW_TIMES; i++)
	{
		sys_move_cursor(1, curs+i+1);
		scanf(&mem1);
		sys_move_cursor(1, curs+i+1);
		memory[i+RW_TIMES] = *(int *)mem1;
		if(memory[i+RW_TIMES] == memory[i])
			printf("Read succeed: %d", memory[i+RW_TIMES]);
		else
			printf("Read error: %d", memory[i+RW_TIMES]);
	}
	while(1);
	//Only input address.
	//Achieving input r/w command is recommended but not required.
}

void rw_task2(void)
{
	int mem[10] ;
	mem[0] = address + 0x2000;
	address += 0x2000;
	mem[1] = address + 0x2000;
	address += 0x2000;
	mem[2] = address + 0x2000;
	address += 0x2000;
	mem[3] = address + 0x2000;
	address += 0x2000;
	mem[4] = address + 0x2000;
	address += 0x2000;
	mem[5] = address + 0x2000;
	address += 0x2000;
	mem[6] = address + 0x2000;
	address += 0x2000;
	mem[7] = address + 0x2000;
	address += 0x2000;
	mem[8] = address + 0x2000;
	address += 0x2000;
	mem[9] = address + 0x2000;
	int mem2 = 2333;
	int curs = 0;
	int memory[8];
	sys_move_cursor(0, 0);
	printf("                                          ");
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		sys_move_cursor(1, curs+i);
		memory[i] = mem2 = rand();
		sys_move_cursor(1, curs+i);
		printf("Write: 0x%x: %d\n", mem[i], mem2);
		*(int *)mem[i] = mem2;
		//sys_sleep(1);
	}
	curs = 6;
	for(i = 0; i < 8; i++)
	{
		sys_move_cursor(1, curs+i+1);
		memory[i+8] = *(int *)mem[i];
		sys_move_cursor(1, curs+i+1);
		if(memory[i+8] == memory[i])
			printf("Read succeed: 0x%x: %d\n",mem[i], memory[i+8]);
		else
			printf("Read error: 0x%x: %d\n",mem[i], memory[i+8]);
		//sys_sleep(1);
	}
	sys_spawn(test_tasks[3]);
	sys_waitpid(current_running->pid + 1);
	curs = 6;
	for(i = 0; i < 8; i++)
	{
		sys_move_cursor(1, curs+i+1);
		printf("-------------------------                ");
	}
	curs = 6;
	for(i = 0; i <8; i++)
	{
		sys_move_cursor(1, curs+i+1);
		memory[i+8] = *(int *)mem[i];
		sys_move_cursor(1, curs+i+1);
		if(memory[i+8] == memory[i])
			printf("Read succeed: 0x%x: %d\n",mem[i], memory[i+8]);
		else
			printf("Read error: 0x%x: %d\n",mem[i], memory[i+8]);
		//sys_sleep(1);
	}
	sys_exit();
}

void rw_task3(void)
{
	int mem[10] ;
	mem[0] = address + 0x2000;
	address += 0x2000;
	mem[1] = address + 0x2000;
	address += 0x2000;
	mem[2] = address + 0x2000;
	address += 0x2000;
	mem[3] = address + 0x2000;
	address += 0x2000;
	mem[4] = address + 0x2000;
	address += 0x2000;
	mem[5] = address + 0x2000;
	address += 0x2000;
	mem[6] = address + 0x2000;
	address += 0x2000;
	mem[7] = address + 0x2000;
	address += 0x2000;
	mem[8] = address + 0x2000;
	address += 0x2000;
	mem[9] = address + 0x2000;
	int mem2 = 2333;
	int curs = 0;
	int memory[8];
	sys_move_cursor(0, 0);
	printf("                                          ");
	int i = 0;
	for(i = 0; i < 8; i++)
	{
		sys_move_cursor(1, curs+i);
		memory[i] = mem2 = rand();
		sys_move_cursor(1, curs+i);
		*(int *)mem[i] = mem2;
		printf("Write: 0x%x: %d\n", mem[i], mem2);
		//sys_sleep(1);
	}
	curs = 6;
	for(i = 0; i < 8; i++)
	{
		sys_move_cursor(1, curs+i+1);
		memory[i+8] = *(int *)mem[i];
		sys_move_cursor(1, curs+i+1);
		if(memory[i+8] == memory[i])
			printf("Read succeed: 0x%x: %d\n",mem[i], memory[i+8]);
		else
			printf("Read error: 0x%x: %d\n",mem[i], memory[i+8]);
		//sys_sleep(1);
	}
	sys_exit();
	//while(1);
}