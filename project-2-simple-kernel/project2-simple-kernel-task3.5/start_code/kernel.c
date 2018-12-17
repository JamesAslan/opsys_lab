#define PORT 0xbfe48000
#include "sched.h"
#include "task.h"

void (*BIOS_printstr)(char *b) = 0x8007b980;

void PCB_init(int target)
{
	int j;
	for(j=0;j<32;j++)
	{
		pcb[target].user_context.regs[j]=0;
	}
	pcb[target].user_context.pc =0;
	pcb[target].user_context.lo =0;
	pcb[target].user_context.hi =0;
	pcb[target].user_context.cp0_status =0;
	pcb[target].user_context.cp0_epc =0;
	pcb[target].user_context.cp0_cause =0;
	pcb[target].user_context.cp0_badvaddr =0;

	for(j=0;j<32;j++)
	{
		pcb[target].kernel_context.regs[j]=0;
	}
	pcb[target].kernel_context.pc =0;
	pcb[target].kernel_context.lo =0;
	pcb[target].kernel_context.hi =0;
	pcb[target].kernel_context.cp0_status =0;
	pcb[target].kernel_context.cp0_epc =0;
	pcb[target].kernel_context.cp0_cause =0;
	pcb[target].kernel_context.cp0_badvaddr =0;

	pcb[target].user_stack_top = 0xa0f00000+;
	pcb[target].kernel_stack_top = 0xa0f00000;

	pcb[target].prev = NULL;
	pcb[target].next = NULL;

	pcb[target].pid = target;

	pcb[target].type = KERNEL_PROCESS;

	pcb[target].status = TASK_EXITED;

	pcb[target].cursor_x = 0;
	pcb[target].cursor_y = 0;
	printk("1\n");
}

void __attribute__((section(".entry_function"))) _start(void)
{
	// Call PMON BIOS printstr to print message "Hello OS!"
	char a[]="Hello OS!";
	BIOS_printstr(a);
	
	//PCB_init
	int i;
	for(i=0;i<NUM_MAX_TASK;i++)
	{
		PCB_init(i);
	}
	pcb[0].type = sched1_tasks[0]->type;
	pcb[0].kernel_context.regs[31] = sched1_tasks[0]->entry_point;
	pcb[0].status = TASK_READY;
	
	pcb[1].type = sched1_tasks[1]->type;
	pcb[1].kernel_context.regs[31] = sched1_tasks[1]->entry_point;
	pcb[1].status = TASK_READY;
	
	pcb[2].type = sched1_tasks[2]->type;
	pcb[2].kernel_context.regs[31] = sched1_tasks[2]->entry_point;
	pcb[2].status = TASK_READY;

	printk("2\n");

	do_scheduler();

	//printstr(b);
	return;
}

