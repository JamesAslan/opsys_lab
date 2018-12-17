#include "barrier.h"
#include "sched.h"

void do_barrier_init(barrier_t *barrier, int goal)
{
    barrier->goal =goal;
    barrier->symbol =0;
    queue_init(&barrier->queue);
}

void do_barrier_wait(barrier_t *barrier)
{
    barrier->symbol ++;
    if(barrier->symbol >= barrier->goal)
    {
        while(queue_is_empty(&barrier->queue)==0)
        {
            do_unblock_one(&barrier->queue);
        }
        barrier->symbol =0;
    }
    else
    {
        do_block(&barrier->queue);
        do_scheduler();
    }
}