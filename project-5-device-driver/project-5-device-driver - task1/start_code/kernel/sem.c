#include "sem.h"
#include "stdio.h"
#include "sched.h"

void do_semaphore_init(semaphore_t *s, int val)
{
    queue_init(&s->queue);
    s->value = val;
}

void do_semaphore_up(semaphore_t *s)
{
    s->value++;
    if(s->value>= 0)
    {
        if(queue_is_empty(&s->queue)==0)
        {
            do_unblock_one(&s->queue);
        }
    }
}

void do_semaphore_down(semaphore_t *s)
{
    //if(s->value >=0)
        s->value --;
    if(s->value <0)
    {
        do_block(&s->queue);
        do_scheduler();
    }
}