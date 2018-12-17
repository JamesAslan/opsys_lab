#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"

int exception_handler[32];

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    time_elapsed++;
    //printk("%d\n",time_elapsed);
    if(time_elapsed %200 == 0)
    {
        
        int i;
        if(current_running -> status == TASK_RUNNING)
            current_running -> status = TASK_READY;
        if(queue_is_empty(&ready_queue_high)==0)
        {
            current_running = queue_dequeue(&ready_queue_high);
            current_running -> status = TASK_RUNNING;
            //printk("%d\n",current_running ->pid);
        }
        else if(queue_is_empty(&ready_queue)==0)
        {
            current_running = queue_dequeue(&ready_queue);
            current_running -> status = TASK_RUNNING;
            //printk("%d\n",current_running ->pid);
        } 

        for (i=0;i<NUM_MAX_TASK;i++)
        {
            if(pcb[i].status == TASK_READY)
            {
                if(pcb[i].priority == 1)
                    queue_push(&ready_queue_high,&pcb[i]);
                queue_push(&ready_queue,&pcb[i]);
            }
        }
    } 
    cp0_count_remove();
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    int cause1 = cause & 0x0000ff00;
    if(cause1 == 0x00008000)
    {
        irq_timer();
    }
}

void other_exception_handler()
{
    // TODO other exception handler
}