#include "mm.h"

//TODO:Finish memory management functions here refer to mm.h and add any functions you need.
void init_page_table( void )
{
    PTE_info_clear();
    TLB_info_clear();
    int i;
    for (i=0;i< TLB_ENTRY_NUMBER*100;i++)
    {
        PTE_info[i].id=i;
        PTE_info[i].virtue = 0x00000000 + i*1024*4*8;
        PTE_info[i].real = 0x000080000 + i*1024*4*8;
        PTE_info[i].valid = 1;
        PTE_info[i].dirty = 1;
        PTE_info[i].coherency = 2;
        PTE_info[i].global = 1;
    }
    for (i=0;i< 32;i++)
    {
        TLB_load(2*i,2*i+1);
    }
}

void TLB_load(int pte0, int pte1)
{
    int asid = 0;
    int hi_input = (PTE_info[pte0].virtue & 0xe000) | (asid & 0xff);
    //int coherency0,coherency1;
    //int dirty0,dirty1;
    //int valid0,valid1;
    //int global0,global1;
    int lo1_input = (PTE_info[pte1].real & 0xc0) | (PTE_info[pte1].coherency << 3) | (PTE_info[pte1].dirty << 2) | (PTE_info[pte1].valid << 1) | PTE_info[pte1].global;
    int lo0_input = (PTE_info[pte0].real & 0xc0) | (PTE_info[pte0].coherency << 3) | (PTE_info[pte0].dirty << 2) | (PTE_info[pte0].valid << 1) | PTE_info[pte0].global;
    int index_input = current_TLB++;
    TLB_info_load((int)hi_input,(int)lo0_input,(int)lo1_input,(int)index_input);
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