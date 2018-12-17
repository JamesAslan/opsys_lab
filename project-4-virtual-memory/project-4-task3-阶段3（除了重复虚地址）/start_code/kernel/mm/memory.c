#include "mm.h"
#include "sched.h"
#include "irq.h"

//TODO:Finish memory management functions here refer to mm.h and add any functions you need.
void init_page_table( void )
{
    PTE_info_clear();
    TLB_info_clear();
    int i;
    for (i=0;i< PTE_NUMBER;i++)
    {
        PTE_info[i].asid= 0;
        PTE_info[i].virtue = 0x00000000 + i*1024*4;
        //PTE_info[i].real = 0x00080000 + i*1024*4;
        PTE_info[i].valid = 0;
        PTE_info[i].usage = 0;
        PTE_info[i].dirty = 1;
        PTE_info[i].coherency = 2;
        PTE_info[i].global = 1;
    }
    /*for (i=0;i< 32;i++)
    {
        TLB_load(2*i,2*i+1,0,current_TLB++);
    }*/
}

void TLB_load(int pte0, int pte1,int asid,int index)
{
    //int asid = 0;
    int hi_input = (PTE_info[pte0].virtue & 0xffffe000)| (asid & 0xff);
    //int coherency0,coherency1;
    //int dirty0,dirty1;
    //int valid0,valid1;
    //int global0,global1;
    int lo1_input = ((PTE_info[pte1].real & 0xfffff000)>>6) | (PTE_info[pte1].coherency << 3) | (PTE_info[pte1].dirty << 2) | (PTE_info[pte1].valid << 1) | PTE_info[pte1].global;
    int lo0_input = ((PTE_info[pte0].real & 0xfffff000)>>6) | (PTE_info[pte0].coherency << 3) | (PTE_info[pte0].dirty << 2) | (PTE_info[pte0].valid << 1) | PTE_info[pte0].global;
    //int lo1_input = (PTE_info[pte1].real << 6) | (PTE_info[pte1].coherency << 3) | (PTE_info[pte1].dirty << 2) | (PTE_info[pte1].valid << 1) | PTE_info[pte1].global;
    //int lo0_input = (PTE_info[pte0].real << 6) | (PTE_info[pte0].coherency << 3) | (PTE_info[pte0].dirty << 2) | (PTE_info[pte0].valid << 1) | PTE_info[pte0].global;
    //int index_input = current_TLB++;
    //TLB_info_load((int)hi_input,(int)lo0_input,(int)lo1_input,(int)index_input);
    //printk("TLB:-----%x,%x---------",PTE_info[pte0].virtue,index);
    TLB_info_load(hi_input,lo0_input,lo1_input,index);
}

void TLB_info_clear(void)
{
    current_real_ad =0;
    current_TLB = 0;
    int i;
    for(i=0;i< TLB_ENTRY_NUMBER;i++)
    {
        TLB_info[i].usage=0;
        TLB_info[i].valid=0;
    }
}

void PTE_info_clear(void)
{
    int i;
    for(i=0;i< PTE_NUMBER;i++)
    {
        PTE_info[i].usage = 0;
    }
}

void TLB_miss(int context)
{
    //my_printk("miss!");
    int i;
    //my_printk("context:%x!",context);
    int target = context & 0xffffe000;
    //my_printk("target:%x!",target);
    //printk("target:%x!",target);
    int index_input;
    int asid_input = current_running->pid;
    if(current_TLB >= TLB_ENTRY_NUMBER)
        current_TLB =0;
    index_input = current_TLB;
    for(i=0;i<PTE_NUMBER;i++)
    {
        if((PTE_info[i].virtue & 0xffffe000) == target)
        {
            //if((PTE_info[i].usage == 1) /*&& (PTE_info[i].asid == asid_input)*/)
            //{
                //printk("loading%d! virtue:%x index:%x",i,PTE_info[i].virtue,index_input);
                TLB_load(i,i+1,asid_input,index_input);
                break;
        }
    }
    current_TLB++;
}

void TLB_invalid(int context,int index)
{
    my_printk("invalid!");
    int i;
    my_printk("context:%x!",context);
    int target = context & 0xffffe000;
    //printk("target:%x!",target);
    int index_input = index & 0x0000003f;
    int asid_input = current_running->pid;
    for(i=0;i<PTE_NUMBER;i++)
    {
        if(((PTE_info[i].virtue & 0xffffe000) == target))
        {
            /*if((PTE_info[i].valid == 1) && (PTE_info[i].usage == 1) && (PTE_info[i].asid == asid_input))
            {

                printk("miss loading%d!",i);
                TLB_load(i,i+1,asid_input,index_input);
                break;
            }
            else*/ 
            if (PTE_info[i].usage == 0)
            {
                my_printk("locating and to be loaded%d!",i);
                PTE_info[i].real = 0x00080000 + current_real_ad*1024*4;
                PTE_info[i].usage =1;
                PTE_info[i].valid =1;
                current_real_ad++;
                PTE_info[i+1].real = 0x00080000 + current_real_ad*1024*4;
                PTE_info[i+1].usage =1;
                PTE_info[i+1].valid =1;
                current_real_ad++;
                TLB_load(i,i+1,asid_input,index_input);
                break;
            }
        }
    }
}

void TLB_helper(int context,int index,int badaddr)
{
    //my_printk("badaddr:%x;context:%x",badaddr);
    int context_input = context << 9;
    if((index & 0x80000000) == 0)
        TLB_invalid(badaddr,index);
    else
        TLB_miss(badaddr);

}