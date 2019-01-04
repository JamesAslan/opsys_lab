#include "mac.h"
#include "irq.h"
#include "type.h"
#include "screen.h"
#include "syscall.h"
#include "sched.h"
#include "test4.h"
#include "dma.h"
#include "time.h"

desc_t *send_desc;
desc_t *receive_desc;
uint32_t cnt = 1; //record the time of iqr_mac
//uint32_t buffer[PSIZE] = {0x00040045, 0x00000100, 0x5d911120, 0x0101a8c0, 0xfb0000e0, 0xe914e914, 0x00000801,0x45000400, 0x00010000, 0x2011915d, 0xc0a80101, 0xe00000fb, 0x14e914e9, 0x01080000};
uint32_t buffer[PSIZE] = {0xffffffff, 0x5500ffff, 0xf77db57d, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0004e914, 0x0000, 0x005e0001, 0x2300fb00, 0x84b7f28b, 0x00450008, 0x0000d400, 0x11ff0040, 0xa8c073d8, 0x00e00101, 0xe914fb00, 0x0801e914, 0x0000};

/**
 * Clears all the pending interrupts.
 * If the Dma status register is read then all the interrupts gets cleared
 * @param[in] pointer to synopGMACdevice.
 * \return returns void.
 */

queue_t recv_block_queue;
uint32_t recv_flag[PNUM];
uint32_t ch_flag;

void clear_interrupt()
{
    uint32_t data;
    data = reg_read_32(0xbfe11000 + DmaStatus);
    reg_write_32(0xbfe11000 + DmaStatus, data);
}

static void send_desc_init(mac_t *mac)
{
    int i;
    for(i=0;i<64;i++)
        memcpy(dma_buffer_start_sd + 1024*i,&buffer,1024);
    mac->td_phy = dma_buffer_start_sd & 0x1fffffff;
    mac->td = &sd_desc;
}

static void recv_desc_init(mac_t *mac)
{
    mac->rd_phy = dma_buffer_start_rd & 0x1fffffff;
    mac->rd = &rd_desc;
}


static void mii_dul_force(mac_t *mac)
{
    reg_write_32(mac->dma_addr, 0x80); //?s
                                       //   reg_write_32(mac->dma_addr, 0x400);
    uint32_t conf = 0xc800;            //0x0080cc00;

    // loopback, 100M
    reg_write_32(mac->mac_addr, reg_read_32(mac->mac_addr) | (conf) | (1 << 8));
    //enable recieve all
    reg_write_32(mac->mac_addr + 0x4, reg_read_32(mac->mac_addr + 0x4) | 0x80000001);
}

static void start_tran(mac_t *mac)
{

   
}

void dma_control_init(mac_t *mac, uint32_t init_value)
{
    reg_write_32(mac->dma_addr + DmaControl, init_value);
    return;
}

void phy_regs_task1()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t print_location = 2;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum

    send_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    //register_irq_handler(LS1C_MAC_IRQ, irq_mac);

    //irq_enable(LS1C_MAC_IRQ);
    sys_move_cursor(1, print_location);
    printf("> [SEND TASK] start send package.               \n");

    uint32_t cnt = 0;
    i = 4;
    while (i > 0)
    {
        sys_net_send(test_mac.td, test_mac.td_phy);
        cnt += PNUM;
        sys_move_cursor(1, print_location);
        printf("> [SEND TASK] totally send package %d !        \n", cnt);
        i--;
    }
    sys_exit();
}

void phy_regs_task2()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t ret;
    uint32_t print_location = 1;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum
    recv_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    queue_init(&recv_block_queue);
    sys_move_cursor(1, print_location);
    printf("[RECV TASK] start recv:                    ");
    ret = sys_net_recv(test_mac.rd, test_mac.rd_phy, test_mac.daddr);
  
    ch_flag = 0;
    for (i = 0; i < PNUM; i++)
    {
        recv_flag[i] = 0;
    }

    uint32_t cnt = 0;
    uint32_t *Recv_desc;
    Recv_desc = (uint32_t *)(test_mac.rd + (PNUM - 1) * 16);
    //printf("(test_mac.rd 0x%x ,Recv_desc=0x%x,REDS0 0X%x\n", test_mac.rd, Recv_desc, *(Recv_desc));
    if (((*Recv_desc) & 0x80000000) == 0x80000000)
    {
        sys_move_cursor(1, print_location);
        printf("> [RECV TASK] waiting receive package.\n");
        sys_wait_recv_package();
    }
    check_recv(&test_mac);

    sys_exit();
}

void phy_regs_task3()
{
    uint32_t print_location = 1;
    sys_move_cursor(1, print_location);
    printf("> [INIT] Waiting for MAC initialization .\n");
    sys_init_mac();
    sys_move_cursor(1, print_location);
    printf("> [INIT] MAC initialization succeeded.           \n");
    sys_exit();
}

void phy_regs_task4()
{

    mac_t test_mac;
    uint32_t i;
    uint32_t ret;
    uint32_t print_location = 1;

    test_mac.mac_addr = 0xbfe10000;
    test_mac.dma_addr = 0xbfe11000;

    test_mac.psize = PSIZE * 4; // 64bytes
    test_mac.pnum = PNUM;       // pnum
    recv_desc_init(&test_mac);

    dma_control_init(&test_mac, DmaStoreAndForward | DmaTxSecondFrame | DmaRxThreshCtrl128);
    clear_interrupt(&test_mac);

    mii_dul_force(&test_mac);

    queue_init(&recv_block_queue);
    sys_move_cursor(1, print_location);
    printf("[RECV TASK] start recv:                    ");
  
    ch_flag = 0;
    for (i = 0; i < PNUM; i++)
    {
        recv_flag[i] = 0;
    }

    /*uint32_t cnt = 0;
    uint32_t *Recv_desc;
    Recv_desc = (uint32_t *)(test_mac.rd + (PNUM - 1) * 16);*/
    int j=0;
    //printf("(test_mac.rd 0x%x ,Recv_desc=0x%x,REDS0 0X%x\n", test_mac.rd, Recv_desc, *(Recv_desc));
    int time_start = time_elapsed;
    /*while(mac_store_current < mac_store_start + 0x004000000x00800000)*/
    ret = sys_net_recv(test_mac.rd, test_mac.rd_phy, test_mac.daddr);
    while(mac_counter<64)
    {
        if ((rd_desc[127].tdes0 & 0x80000000) == 0x80000000)
        {
            //sys_move_cursor(1, print_location);
            //printf("> [RECV TASK] waiting receive package(%d).  ",j);
            sys_wait_recv_package();
        }
        //printf("> [RECV TASK] the rest is %x(%d).\n",mac_store_start + 0x00200000 - mac_store_current,j);
        /*for(i=0;i<64;i++)
            memcpy(mac_store_current + 1024*i,dma_buffer_start_rd + 1024*i,1024);*/
        
        //printf("done (%d).\n",j);
    }
    int time_end = time_elapsed;
    printf("> [RECV TASK] total: %dKB, time: %d irq_time.\n",mac_counter*128,time_end-time_start);
    my_check_recv(&test_mac);

    sys_exit();
}

