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
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    if (lock->status == LOCKED)
    {
        do_block(&lock->queue);
        scheduler();
    }
    else
        lock->status = LOCKED;

}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    //do_unblock_all(&lock->queue);
    //lock->status == UNLOCKED;
    if(queue_is_empty(&lock->queue)==0)
    {
        do_unblock_one(&lock->queue);
        lock->status = LOCKED; 
    }
    else
        lock->status = UNLOCKED;   

    scheduler(); 
}
