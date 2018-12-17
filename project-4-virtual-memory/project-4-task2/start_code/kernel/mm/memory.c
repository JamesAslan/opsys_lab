#include "mm.h"

//TODO:Finish memory management functions here refer to mm.h and add any functions you need.
void init_page_table( void )
{
    PTE_info_clear();
    TLB_info_clear();
    int i;
    for (i=0;i< PTE_NUMBER;i++)
    {
        PTE_info[i].id=i;
        PTE_info[i].virtue = 0x00000000 + i*1024*4;
        PTE_info[i].real = 0x00080000 + i*1024*4;
        PTE_info[i].valid = 1;
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
    int hi_input = (PTE_info[pte0].virtue & 0xe000) | (asid & 0xff);
    //int coherency0,coherency1;
    //int dirty0,dirty1;
    //int valid0,valid1;
    //int global0,global1;
    int lo1_input = (PTE_info[pte1].real & 0xc0) | (PTE_info[pte1].coherency << 3) | (PTE_info[pte1].dirty << 2) | (PTE_info[pte1].valid << 1) | PTE_info[pte1].global;
    int lo0_input = (PTE_info[pte0].real & 0xc0) | (PTE_info[pte0].coherency << 3) | (PTE_info[pte0].dirty << 2) | (PTE_info[pte0].valid << 1) | PTE_info[pte0].global;
    //int index_input = current_TLB++;
    //TLB_info_load((int)hi_input,(int)lo0_input,(int)lo1_input,(int)index_input);
    TLB_info_load((int)hi_input,(int)lo0_input,(int)lo1_input,(int)index);
}

void TLB_info_clear(void)
{
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
    int i;
    int target = context & 0xffffe000;
    int index = current_TLB++;
    if(current_TLB >= TLB_ENTRY_NUMBER/2)
        current_TLB =0;
    for(i=0;i<PTE_NUMBER;i++)
    {
        if((PTE_info[i].valid == 1) && ((PTE_info[i].virtue & 0xffffe000) == target))
        {
            printk("miss loading%d!",i);
            TLB_load(i,i+1,0,index);
            break;
        }
    }
}

void TLB_invalid(int context,int index)
{
    printk("=-=");
}

void TLB_helper(int context,int index)
{
    if((index & 0x80000000) == 1)
        TLB_invalid(context,index);
    else
        TLB_miss(context);

}