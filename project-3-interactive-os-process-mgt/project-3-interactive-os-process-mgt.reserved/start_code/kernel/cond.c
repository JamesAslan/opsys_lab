#include "cond.h"
#include "lock.h"
#include "queue.h"

void do_condition_init(condition_t *condition)
{
    queue_init(&(condition->queue));
}

void do_condition_wait(mutex_lock_t *lock, condition_t *condition)
{
    do_mutex_lock_release(lock);
    do_block(&(condition->queue));
    do_scheduler();
}

void do_condition_signal(condition_t *condition)
{
    if(queue_is_empty(&(condition->queue))==0)
    {
        do_unblock_one(&(condition->queue));
    }
}

void do_condition_broadcast(condition_t *condition)
{
    while(queue_is_empty(&(condition->queue))==0)
    {
        do_unblock_one(&(condition->queue));
    }
}