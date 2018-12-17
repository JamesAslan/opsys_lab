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
        scheduler();
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
