#include "dma.h"
#include "queue.h"

struct desc rd_desc[64];
struct desc sd_desc[64];

int dma_buffer_start_rd = 0x80e00000;
int dma_buffer_start_sd = 0x80e40000;

queue_t mac_wait;

void rd_desc_init(void)
{
    int i;
    int target;
    for(i=0 ; i< 63; i++)
    {
        rd_desc[i].tdes0 = 0;
        rd_desc[i].tdes1 = 0x81000400;
        target = dma_buffer_start_rd + 1024*i;
        rd_desc[i].tdes2 = target & 0x1fffffff;
        target = &rd_desc[i+1];
        rd_desc[i].tdes3 = target & 0x1fffffff;
    }
    rd_desc[i].tdes0 = 0;
    rd_desc[i].tdes1 = 0x03000400;
    target = dma_buffer_start_rd + 1024*63;
    rd_desc[i].tdes2 = target & 0x1fffffff;
    target = &rd_desc;
    rd_desc[i].tdes3 = target & 0x1fffffff;
}

void sd_desc_init(void)
{
    int i;
    int target;
    for(i=0 ; i< 63; i++)
    {
        sd_desc[i].tdes0 = 0;
        sd_desc[i].tdes1 = 0x61000400;
        target = dma_buffer_start_sd + 1024*i;
        sd_desc[i].tdes2 = target & 0x1fffffff;
        target = &sd_desc[i+1];
        sd_desc[i].tdes3 = target & 0x1fffffff;
    }
    sd_desc[i].tdes0 = 0;
    sd_desc[i].tdes1 = 0xe3000400;
    target = dma_buffer_start_sd + 1024*63;
    sd_desc[i].tdes2 = target & 0x1fffffff;
    target = &sd_desc;
    sd_desc[i].tdes3 = target & 0x1fffffff;
}

void dma_buffer_clear(void)
{
    int i;
    for(i=0;i<256*64;i++)
    {
        *((int*)dma_buffer_start_rd) = 0;
    }
    //*((int*)0xa0000000) = 0;
}

void check_mac(void)
{
    if(queue_is_empty(&mac_wait) == 0)
    {
        if((rd_desc[63].tdes0 & 0x80000000) == 0x00000000)
        {
            //my_printk("unblock!");
            do_unblock_one(&mac_wait);
        }
    }
}