#include "lock.h"
#include "cond.h"
#include "time.h"
#include "stdio.h"
#include "test3.h"
#include "syscall.h"
#include "time.h"

static mutex_lock_t mutex;
static condition_t condition;
static int num_staff = 0;

void producer_task(void)
{
    condition_init(&condition);
    mutex_lock_init(&mutex);
    int i;
    int print_location = 0;
    int production = 3;
    int sum_production = 0;

    for (i = 0; i < 50; i++)
    {
        mutex_lock_acquire(&mutex);

        num_staff += production;
        sum_production += production;

        mutex_lock_release(&mutex);

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total produced %d products.", sum_production);

        //condition_signal(&condition);
        condition_broadcast(&condition);

        sys_sleep(1);
    }

    sys_exit();
}

void consumer_task1(void)
{
    int print_location = 1;
    int consumption = 1;
    int sum_consumption = 0;

    while (1)
    {
        mutex_lock_acquire(&mutex);

        while (num_staff == 0)
        {
            printf("consumer_task1 %d",num_staff);
            condition_wait(&mutex, &condition);
        }

        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products.%d", sum_consumption,num_staff);

        mutex_lock_release(&mutex);
    }
}

void consumer_task2(void)
{
    int print_location = 2;
    int consumption = 1;
    int sum_consumption = 0;

    while (1)
    {
        mutex_lock_acquire(&mutex);

        while (num_staff == 0)
        {
            printf("consumer_task2 %d",num_staff);
            condition_wait(&mutex, &condition);
        }
        printf("consumer_task2 %d",num_staff);
        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products.%d", sum_consumption,num_staff);

        mutex_lock_release(&mutex);
    }
}