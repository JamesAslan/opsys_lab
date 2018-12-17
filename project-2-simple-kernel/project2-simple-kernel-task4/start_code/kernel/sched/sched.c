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

static void check_sleeping()
{
}

void scheduler(void)
{
    int i;
    //(*current_running).status = TASK_EXITED;
    if(queue_is_empty(&ready_queue)==0)
    {
        current_running = queue_dequeue(&ready_queue);
        current_running -> status = TASK_RUNNING;
    } 

    for (i=0;i<NUM_MAX_TASK;i++)
    {
        if(pcb[i].status == TASK_READY)
        {
            queue_push(&ready_queue,&pcb[i]);
        }
    }

    // TODO schedule
    // Modify the current_running pointer.
}

void do_sleep(uint32_t sleep_time)
{
    // TODO sleep(seconds)
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
    //current_running -> status = TASK_EXITED;
    item = queue_dequeue(queue);
    item -> status = TASK_READY;
}

void do_unblock_all(queue_t *queue)
{
    // unblock all task in the queue
    while(queue_is_empty(queue)==0)
    {
        pcb_t *item;
        item = queue_dequeue(queue);
        item -> status = TASK_READY;
    }
}
