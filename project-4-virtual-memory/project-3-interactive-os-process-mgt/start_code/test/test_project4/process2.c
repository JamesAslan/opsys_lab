#include "sched.h"
#include "stdio.h"
#include "syscall.h"
#include "time.h"
#include "screen.h"
#include "test4.h"

#define RW_TIMES 2

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
	int cur_x=2;
	while (1)
	{
    	char ch = read_uart_ch();
		if(ch <='f' && ch >='a')
			result = result*16 + ch - 'a' + 10;
		else if (ch <='9' && ch >='0')
			result  = result*16 + ch - '0';
		else if(ch != 0 && ch!=13)
		{
			vt100_move_cursor(0,2);
			my_printk("wrong!");
		}
		vt100_move_cursor(0,cur_x);
		cur_x++;
		my_printk("%c",ch);
		if(ch == 13)
			break;
	}
	//printk("3333333333333333333");
	*mem = result;
	printk("%x",result);
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
		sys_move_cursor(1, curs+i);
		scanf(&mem1);
		sys_move_cursor(1, curs+i);
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

