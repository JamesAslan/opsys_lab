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
        //sys_move_cursor(0, 10);
        //printf("000producer:%d        ", sum_production);
        mutex_lock_acquire(&mutex);
        //sys_move_cursor(0, 11);
        //printf("111producer:%d        ", sum_production);
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
        //sys_move_cursor(0, 10);
        //printf("000consumer1:%d        ", sum_consumption);
        mutex_lock_acquire(&mutex);
        //sys_move_cursor(0, 11);
        //printf("111consumer1:%d        ", sum_consumption);
        while (num_staff == 0)
        {
            condition_wait(&mutex, &condition);
        }
        //sys_move_cursor(0, 12);
        //printf("222consumer1:%d       ", sum_consumption);
        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products.", sum_consumption);

        mutex_lock_release(&mutex);
        //sys_move_cursor(0, 13);
        //printf("333consumer1:%d      ", sum_consumption);
    }
}

void consumer_task2(void)
{
    int print_location = 2;
    int consumption = 1;
    int sum_consumption = 0;

    while (1)
    {
        ///sys_move_cursor(0, 10);
        //printf("000consumer2:%d          ", sum_consumption);
        mutex_lock_acquire(&mutex);
        //sys_move_cursor(0, 11);
        //printf("111consumer2:%d          ", sum_consumption);
        while (num_staff == 0)
        {
            condition_wait(&mutex, &condition);
        }
        //sys_move_cursor(0, 12);
        //printf("222consumer2:%d          ", sum_consumption);
        num_staff -= consumption;
        sum_consumption += consumption;

        sys_move_cursor(0, print_location);
        printf("> [TASK] Total consumed %d products", sum_consumption);

        mutex_lock_release(&mutex);
        //sys_move_cursor(0, 13);
        //printf("333consumer2:%d          ", sum_consumption);
    }
}