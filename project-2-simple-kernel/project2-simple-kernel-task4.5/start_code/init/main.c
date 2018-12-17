/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *            Copyright (C) 2018 Institute of Computing Technology, CAS
 *               Author : Han Shukai (email : hanshukai@ict.ac.cn)
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * * * * *
 *         The kernel's entry, where most of the initialization work is done.
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

#include "irq.h"
#include "test.h"
#include "stdio.h"
#include "sched.h"
#include "screen.h"
#include "common.h"
#include "syscall.h"

void PCB_init(int target)
{
	pcb[target].user_stack_top = 0xa0f00000 + 0x00010000 * target;
	pcb[target].kernel_stack_top = 0xa0f00000 + 0x00010000 * target + 0x00005000;

	int j;
	for(j=0;j<32;j++)
	{
		pcb[target].user_context.regs[j]=0;
	}
	pcb[target].user_context.regs[29]=pcb[target].user_stack_top;
	pcb[target].user_context.pc =0;
	pcb[target].user_context.lo =0;
	pcb[target].user_context.hi =0;
	pcb[target].user_context.cp0_status =0x00008000;
	pcb[target].user_context.cp0_epc =0;
	pcb[target].user_context.cp0_cause =0;
	pcb[target].user_context.cp0_badvaddr =0;

	for(j=0;j<32;j++)
	{
		pcb[target].kernel_context.regs[j]=0;
	}
	pcb[target].kernel_context.regs[29]=pcb[target].kernel_stack_top;
	pcb[target].kernel_context.pc =0;
	pcb[target].kernel_context.lo =0;
	pcb[target].kernel_context.hi =0;
	pcb[target].kernel_context.cp0_status =0x00008000;
	pcb[target].kernel_context.cp0_epc =0;
	pcb[target].kernel_context.cp0_cause =0;
	pcb[target].kernel_context.cp0_badvaddr =0;

	pcb[target].prev = NULL;
	pcb[target].next = NULL;

	pcb[target].pid = target;

	pcb[target].type = KERNEL_PROCESS;

	pcb[target].status = TASK_EXITED;

	pcb[target].cursor_x = 0;
	pcb[target].cursor_y = 0;

	pcb[target].priority=0;
	pcb[target].sleep_time_due=0;
	pcb[target].sleep_time_current=0;
}

static void init_pcb()
{
	int i;
	for(i=0;i<NUM_MAX_TASK;i++)
	{
		PCB_init(i);
	}
	/*pcb[0].kernel_context.regs[29]=pcb[0].kernel_stack_top;
	pcb[0].type = sched1_tasks[0]->type;
	pcb[0].kernel_context.regs[31] = sched1_tasks[0]->entry_point;
	pcb[0].status = TASK_READY;
	
	pcb[1].kernel_context.regs[29]=pcb[1].kernel_stack_top;
	pcb[1].type = sched1_tasks[1]->type;
	pcb[1].kernel_context.regs[31] = sched1_tasks[1]->entry_point;
	pcb[1].status = TASK_READY;
	
	pcb[2].kernel_context.regs[29]=pcb[2].kernel_stack_top;
	pcb[2].type = sched1_tasks[2]->type;
	pcb[2].kernel_context.regs[31] = sched1_tasks[2]->entry_point;
	pcb[2].status = TASK_READY;*/

	/*pcb[2].type = lock_tasks[0]->type;
	pcb[2].kernel_context.regs[31] = lock_tasks[0]->entry_point;
	pcb[2].kernel_context.cp0_epc =lock_tasks[0]->entry_point;
	pcb[2].status = TASK_READY;
	
	pcb[3].type = lock_tasks[1]->type;
	pcb[3].kernel_context.regs[31] = lock_tasks[1]->entry_point;
	pcb[3].kernel_context.cp0_epc =lock_tasks[1]->entry_point;
	pcb[3].status = TASK_READY;*/
	
	pcb[0].type = sched2_tasks[0]->type;
	pcb[0].user_context.regs[31] = sched2_tasks[0]->entry_point;
	pcb[0].user_context.cp0_epc =sched2_tasks[0]->entry_point;
	pcb[0].status = TASK_READY;
	
	pcb[1].type = sched2_tasks[1]->type;
	pcb[1].user_context.regs[31] = sched2_tasks[1]->entry_point;
	pcb[1].user_context.cp0_epc =sched2_tasks[1]->entry_point;
	pcb[1].status = TASK_READY;
	
	pcb[4].type = sched2_tasks[2]->type;
	pcb[4].user_context.regs[31] = sched2_tasks[2]->entry_point;
	pcb[4].user_context.cp0_epc =sched2_tasks[2]->entry_point;
	pcb[4].status = TASK_READY;
	//pcb[4].priority=1;

	current_running = &pcb[15];	
	queue_push(&ready_queue,&pcb[0]);
	//queue_push(&ready_queue,&pcb[3]);
	//queue_push(&ready_queue,&pcb[2]);
	queue_push(&ready_queue,&pcb[1]);
	queue_push(&ready_queue,&pcb[4]);
}

static void init_exception_handler()
{
	
}

static void init_exception()
{
	// 1. Get CP0_STATUS
	// 2. Disable all interrupt
	// 3. Copy the level 2 exception handling code to 0x80000180
	// 4. reset CP0_COMPARE & CP0_COUNT register
	cp0_init();

	exception_handler[0] = &handle_int;
	exception_handler[8] = &handle_syscall;

	memcpy(0x80000180,exception_handler_entry,(exception_handler_end-exception_handler_begin));
}

static void init_syscall(void)
{
	// init system call table.
	syscall_handler[SYSCALL_SLEEP] = do_sleep;
	syscall_handler[SYSCALL_BLOCK] = do_block;
	syscall_handler[SYSCALL_UNBLOCK_ONE] = do_unblock_one;
	syscall_handler[SYSCALL_UNBLOCK_ALL] = do_unblock_all;
	syscall_handler[SYSCALL_WRITE] = screen_write;
	syscall_handler[SYSCALL_REFLUSH] = screen_reflush;
	syscall_handler[SYSCALL_CURSOR] = screen_move_cursor;
	syscall_handler[SYSCALL_MUTEX_LOCK_INIT] = do_mutex_lock_init;
	syscall_handler[SYSCALL_MUTEX_LOCK_ACQUIRE] = do_mutex_lock_acquire;
	syscall_handler[SYSCALL_MUTEX_LOCK_RELEASE] = do_mutex_lock_release;
}

// jump from bootloader.
// The beginning of everything >_< ~~~~~~~~~~~~~~
void __attribute__((section(".entry_function"))) _start(void)
{
	// Close the cache, no longer refresh the cache 
	// when making the exception vector entry copy

	asm_start();

	// init interrupt (^_^)
	init_exception();
	printk("> [INIT] Interrupt processing initialization succeeded.\n");

	// init system call table (0_0)
	init_syscall();
	printk("> [INIT] System call initialized successfully.\n");

	// init Process Control Block (-_-!)
	init_pcb();
	printk("> [INIT] PCB initialization succeeded.\n");

	// init screen (QAQ)
	init_screen();
	printk("> [INIT] SCREEN initialization succeeded.\n");

	// TODO Enable interrupt
	cp0_termi();
	//do_scheduler();
	while (1)
	{
		// (QAQQQQQQQQQQQ)
		// If you do non-preemptive scheduling, you need to use it to surrender control
		//do_scheduler();
	}
	return;
}
