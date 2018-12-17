#include "lock.h"
#include "time.h"
#include "stdio.h"
#include "sched.h"
#include "queue.h"
#include "screen.h"

pcb_t pcb[NUM_MAX_TASK];

/* current running task PCB */
pcb_t *current_running;

/* global process id */
pid_t process_id = 1;

queue_t ready_queue;
queue_t ready_queue_high;
queue_t block_queue;

queue_t process_wait[15];

int high_priority_times=0;

void check_sleeping(void)
{
    pcb_t *pcb;
    pcb = block_queue.head;
    while(pcb != NULL)
    {
        if((get_timer() - pcb->sleep_time_current) >= pcb->sleep_time_due)
        {
            queue_remove(&block_queue,pcb);
            pcb->status = TASK_READY;
            pcb->sleep_time_current =0;
            pcb->sleep_time_due =0;
            //if(pcb->priority == 1)                                                                               
            //    queue_push(&ready_queue_high,pcb);
            //else
            //    queue_push(&ready_queue,pcb);
            //while(1);
        }
        pcb = pcb->next;
    }
}

void scheduler(void)
{
        int i;
        current_running->cursor_x = screen_cursor_x;
        current_running->cursor_y = screen_cursor_y;
        if(queue_is_empty(&ready_queue_high)==0 && high_priority_times<=2)
        {
            current_running = queue_dequeue(&ready_queue_high);
            current_running -> status = TASK_RUNNING;
            high_priority_times++;
        }
        else if(queue_is_empty(&ready_queue)==0)
        {
            current_running = queue_dequeue(&ready_queue);
            current_running -> status = TASK_RUNNING;
            high_priority_times=0;
        }
        else
        {
            current_running = &pcb[15];
        } 

        screen_cursor_x = current_running->cursor_x;
        screen_cursor_y = current_running->cursor_y;

        for (i=0;i<NUM_MAX_TASK;i++)
        {
            if(pcb[i].status == TASK_READY)
            {
                if(pcb[i].priority == 1)
                {
                    if(!queue_has_it(&ready_queue_high,&pcb[i]))
                    {    
                        queue_push(&ready_queue_high,&pcb[i]);
                    }
                }
                else if(!queue_has_it(&ready_queue,&pcb[i]))
                {    
                    queue_push(&ready_queue,&pcb[i]);
                }               
            }
        }
    // TODO schedule
    // Modify the current_running pointer.
}

void do_sleep(uint32_t sleep_time)
{
    // TODO sleep(seconds)
    if(current_running -> status == TASK_RUNNING)
    {
        current_running -> status = TASK_BLOCKED;
        current_running -> sleep_time_due = sleep_time;
        current_running -> sleep_time_current = get_timer();
        queue_push(&block_queue,current_running);
        do_scheduler();
    }
}

void do_block(queue_t *queue)
{
    // block the current_running task into the queue
    queue_push(queue,current_running);
    current_running -> status = TASK_BLOCKED;
}

void do_unblock_one(queue_t *queue)
{
    // unblock the head task from the queue
    pcb_t *item;
    item = queue_dequeue(queue);
    item -> status = TASK_READY;
    //queue_push(&ready_queue,item);
}

void do_unblock_all(queue_t *queue)
{
    // unblock all task in the queue
    while(queue_is_empty(queue)==0)
    {
        pcb_t *item;
        item = queue_dequeue(queue);
        item -> status = TASK_READY;
        //queue_push(&ready_queue,item);
    }
}

void my_do_unblock_all(queue_t *queue)
{
    // unblock all task in the queue
    while(queue_is_empty(queue)==0)
    {
        struct component *item;
        item = queue_dequeue(queue);
        //queue_push(&ready_queue,item);
    }
}

int test_point=0;

void process_show (void)
{
    int i;
    int j=0;
    my_printk(" ---[PCB TABLE]---\n");
    for(i=0;i<NUM_MAX_TASK;i++)
    {
        if((pcb[i].status == TASK_READY) || (pcb[i].status == TASK_RUNNING))
        {
            my_printk(" [%d] PID : %d STATUS : RUNNING\n",j,i);
            j++;
        }
    }
    //my_printk("[%d] [%d]\n",screen_cursor_x,screen_cursor_y);
    //while(1);
    test_point =1;
}

int getpid()
{
    return current_running->pid;
}

void spawn(struct task_info *task)
{
    int i=0;
    for(i=0;i<NUM_MAX_TASK;i++)
    {
       if(pcb[i].status == TASK_EXITED)
       {
            pcb[i].type = task->type;
	        pcb[i].user_context.regs[31] = task->entry_point;
	        pcb[i].user_context.cp0_epc = task->entry_point;
	        pcb[i].status = TASK_READY;
            pcb[i].kernel_context.regs[31] = get_addr();
            break;
       }
    }
}

void exit(void)
{
    current_running -> status = TASK_EXITED;
    current_running -> cursor_x = 0;
	current_running -> cursor_y = 0;
    current_running -> kernel_context.regs[29]=current_running -> kernel_stack_top;
	current_running -> kernel_context.pc =0;
	current_running -> kernel_context.lo =0;
	current_running -> kernel_context.hi =0;
    current_running -> kernel_context.cp0_status =0x00008000;
    current_running -> sleep_time_due=0;
	current_running -> sleep_time_current=0;
    do_unblock_all(&process_wait[current_running -> pid]);
    if(current_running ->sleep_time_due !=0)
    {
        queue_remove(&block_queue,current_running);
    }
    if(current_running ->be_locked != NULL)
    {
        queue_remove(&current_running ->be_locked->queue,current_running);
    }
    while(queue_is_empty(&current_running ->resource)==0)
    {
        mutex_lock_t *item;
        item = current_running->resource.head;
        do_mutex_lock_release(item);
    }
    current_running -> prev = NULL;
	current_running -> next = NULL;
    do_scheduler();
}

void wait(pid_t pid)
{
    do_block(&process_wait[pid]);
    do_scheduler();
}

void kill(pid_t pid)
{
    pcb[pid].status = TASK_EXITED;
    pcb[pid].cursor_x = 0;
	pcb[pid].cursor_y = 0;
    pcb[pid].kernel_context.regs[29]=current_running -> kernel_stack_top;
	pcb[pid].kernel_context.pc =0;
	pcb[pid].kernel_context.lo =0;
	pcb[pid].kernel_context.hi =0;
    pcb[pid].kernel_context.cp0_status =0x00008000;
    pcb[pid].sleep_time_due=0;
	pcb[pid].sleep_time_current=0;
    if(queue_has_it(&ready_queue,&pcb[pid]) == 1)
    {
        queue_remove(&ready_queue,&pcb[pid]);
    }
    else if(queue_has_it(&ready_queue_high,&pcb[pid]) == 1)
    {
        queue_remove(&ready_queue_high,&pcb[pid]);
    }
    if(pcb[pid].sleep_time_due !=0)
    {
        queue_remove(&block_queue,&(pcb[pid]));
    }
    if(pcb[pid].be_locked != NULL)
    {
        queue_remove(&(pcb[pid].be_locked->queue),&(pcb[pid]));
    }
    while(queue_is_empty(&(pcb[pid].resource))==0)
    {
        mutex_lock_t *item;
        item = pcb[pid].resource.head;
        my_mutex_lock_release(item,pid);
    }
    do_unblock_all(&process_wait[pid]);
    pcb[pid].prev = NULL;
	pcb[pid].next = NULL;
}