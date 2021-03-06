#include "irq.h"
#include "time.h"
#include "sched.h"
#include "screen.h"
#include "string.h"
#include "dma.h"

uint32_t exception_handler[32];

void (*syscall_handler[64])(int, int, int);

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    time_elapsed++;
    //printk("%d\n",time_elapsed);
    //check_sleeping();
    //screen_reflush();
    //if(time_elapsed %20 == 0)
    //{
        check_sleeping();
        //check_mac();
        if(current_running -> status == TASK_RUNNING)
            current_running -> status = TASK_READY;
        do_scheduler();
    //} 
    //cp0_count_remove();
    //screen_reflush();
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    int cause1 = cause & 0x0000ff00;
    if(cause1 == 0x00008000)
    {
        irq_timer();
        cp0_count_remove();
    }
    else if(cause1 = 0x00000800)
    {
        irq_device();
    }
}

void other_exception_handler()
{
    // TODO other exception handler
}

void irq_device(void)
{
    //my_printk("irq_device");
    int irq_mac_whether=reg_read_32(0xbfd01058);
    if((irq_mac_whether & 0x00000008) == 0x00000008)
        irq_mac();
}