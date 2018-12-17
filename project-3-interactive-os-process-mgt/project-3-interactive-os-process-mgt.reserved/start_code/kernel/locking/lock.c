#include "lock.h"
#include "sched.h"
#include "syscall.h"

void spin_lock_init(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void spin_lock_acquire(spin_lock_t *lock)
{
    while (LOCKED == lock->status)
    {
    };
    lock->status = LOCKED;
}

void spin_lock_release(spin_lock_t *lock)
{
    lock->status = UNLOCKED;
}

void do_mutex_lock_init(mutex_lock_t *lock)
{
    queue_init(&lock->queue);
    lock->status = UNLOCKED;
    lock->prev = NULL;
    lock->next = NULL;
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    if (lock->status == LOCKED)
    {
        do_block(&lock->queue);
        current_running ->be_locked = lock;
        scheduler();
    }
    else
    {
        lock->status = LOCKED;
        my_queue_push(&current_running ->resource,lock);
    }
}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    //do_unblock_all(&lock->queue);
    //lock->status == UNLOCKED;
    my_queue_remove(&current_running ->resource,lock);

    if(queue_is_empty(&lock->queue)==0)
    {
        pcb_t *item =(lock->queue).head;
        item->be_locked =0;
        do_unblock_one(&lock->queue);
        lock->status = LOCKED; 
    }
    else
        lock->status = UNLOCKED;   

    //scheduler(); 
}

void my_mutex_lock_release(mutex_lock_t *lock, int pid)
{
    my_queue_remove(&(pcb[pid].resource),lock);

    if(queue_is_empty(&lock->queue)==0)
    {
        pcb_t *item =(lock->queue).head;
        item->be_locked =0;
        do_unblock_one(&lock->queue);
        lock->status = LOCKED; 
    }
    else
        lock->status = UNLOCKED;   
}
