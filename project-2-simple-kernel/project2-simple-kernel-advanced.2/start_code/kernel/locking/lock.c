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
    lock->pointer = -1;
    lock->status = UNLOCKED;
}

void do_mutex_lock_acquire(mutex_lock_t *lock)
{
    if (lock->status == LOCKED)
    {
        current_running -> status = TASK_BLOCKED;
        lock->pointer ++;
        lock->list[lock->pointer]=current_running;
        current_running->lock_times ++;
        scheduler();
    }
    else
        lock->status = LOCKED;

}

void do_mutex_lock_release(mutex_lock_t *lock)
{
    //do_unblock_all(&lock->queue);
    //lock->status == UNLOCKED;
    if(lock->pointer>=0)
    {
        //pcb_t *target;
        (lock->list[0])->lock_times--;
        if((lock->list[0])->lock_times <=0)
            (lock->list[0])->status = TASK_READY;
        //target = lock->list[0];
        int i;
        for(i=0;i<=lock->pointer-1;i++)
        {
            lock->list[i]=lock->list[i+1];
        }
        lock->pointer--;
        lock->status = LOCKED; 
    }
    else
        lock->status = UNLOCKED;   

    //scheduler(); 
}
