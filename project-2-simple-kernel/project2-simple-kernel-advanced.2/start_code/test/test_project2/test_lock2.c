#include "test2.h"
#include "lock.h"
#include "stdio.h"
#include "syscall.h"

int is_init = FALSE;
static char blank[] = {"                                                                "};

/* if you want to use spin lock, you need define SPIN_LOCK */
//  #define SPIN_LOCK
spin_lock_t spin_lock;

/* if you want to use mutex lock, you need define MUTEX_LOCK */
#define MUTEX_LOCK
mutex_lock_t mutex_lock;
mutex_lock_t mutex_lock1;
mutex_lock_t mutex_lock2;

void lock_task1(void)
{
        int print_location = 3;
        while (1)
        {
                int i;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for a lock.          \n");

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location);
                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Has acquired lock and exited.         \n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
#endif
        }
}

void lock_task2(void)
{
        int print_location = 4;
        while (1)
        {
                int i;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for a lock.            \n");

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location);
                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Has acquired lock and exited.       \n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

                
#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
#endif
        }
}

void lock_task3(void)
{
        int print_location = 5;
        while (1)
        {
                int i;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for a lock.            \n");

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock);
#endif

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location);
                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Has acquired lock and exited.       \n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif

                
#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock);
#endif
        }
}

void lock_task4(void)
{
        int print_location = 3;
        int print_location2 = 5;
        while (1)
        {
                int i;
                int j;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock1);
                        mutex_lock_init(&mutex_lock2);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for  lock1.(%d)            \n",j);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock1);
#endif
                sys_move_cursor(1, print_location+1);
                printf("%s", blank);
                sys_move_cursor(1, print_location+1);
                printf("> [TASK] Applying for  lock2.(%d)           \n",j);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock2);
#endif
                sys_move_cursor(1, print_location2);
                printf("%s", blank);

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location2);
                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location2);
                printf("%s", blank);

                sys_move_cursor(1, print_location2);
                printf("> [TASK] Has acquired lock and exited.       \n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif
                j++;
                
#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock1);
                mutex_lock_release(&mutex_lock2);
#endif
        }
}

void lock_task5(void)
{
        int print_location = 6;
        int print_location2 = 8;
        while (1)
        {
                int i;
                int j;
                if (!is_init)
                {

#ifdef SPIN_LOCK
                        spin_lock_init(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                        mutex_lock_init(&mutex_lock1);
                        mutex_lock_init(&mutex_lock2);
#endif
                        is_init = TRUE;
                }

                sys_move_cursor(1, print_location);
                printf("%s", blank);

                sys_move_cursor(1, print_location);
                printf("> [TASK] Applying for  lock1.(%d)            \n",j);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock1);
#endif

                sys_move_cursor(1, print_location+1);
                printf("%s", blank);
                sys_move_cursor(1, print_location+1);
                printf("> [TASK] Applying for  lock2.(%d)           \n",j);

#ifdef SPIN_LOCK
                spin_lock_acquire(&spin_lock);
#endif

#ifdef MUTEX_LOCK
                mutex_lock_acquire(&mutex_lock2);
#endif
                sys_move_cursor(1, print_location2);
                printf("%s", blank);

                for (i = 0; i < 20; i++)
                {
                        sys_move_cursor(1, print_location2);
                        printf("> [TASK] Has acquired lock and running.(%d)\n", i);
                }

                sys_move_cursor(1, print_location2);
                printf("%s", blank);

                sys_move_cursor(1, print_location2);
                printf("> [TASK] Has acquired lock and exited.       \n");

#ifdef SPIN_LOCK
                spin_lock_release(&spin_lock);
#endif
                j++;
                
#ifdef MUTEX_LOCK
                mutex_lock_release(&mutex_lock1);
                mutex_lock_release(&mutex_lock2);
#endif
        }
}