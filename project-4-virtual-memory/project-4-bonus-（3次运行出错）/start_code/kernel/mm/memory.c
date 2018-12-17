#include "mm.h"
#include "sched.h"
#include "irq.h"

int global_i = 0;
int global_j = 0;
//TODO:Finish memory management functions here refer to mm.h and add any functions you need.
void init_page_table( void )
{
    PTE_info_clear();
    TLB_info_clear();
    int i;
    for (i=0;i< PTE_NUMBER;i++)
    {
        PTE_info[i].asid= -1;
        PTE_info[i].virtue = 0x00000000 + i*1024*4;
        //PTE_info[i].real = 0x00090000 /*+ i*1024*4*/;
        PTE_info[i].valid = 0;
        PTE_info[i].usage = 0;
        PTE_info[i].dirty = 1;
        PTE_info[i].coherency = 2;
        PTE_info[i].global = 0;
        PTE_info[i].onsd = 0;
        PTE_info[i].inTLB = 0;
        PTE_info[i].sdaddr = 0;
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
        TLB_info[i].PTE_number=-1;
        //TLB_info[i].valid=0;
    }
}

void PTE_info_clear(void)
{
    swap_target = 4000;
    sd_target = 4*1024;
    current_virtue_ad = PTE_RICH;
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
    int symbol = 0;
    //my_printk("context:%x!",context);
    int target = context & 0xffffe000;
    //my_printk("target:%x!",target);
    //my_printk("target:%x!\n",target);
    int index_input;
    int asid_input = current_running->pid;
    if(current_TLB >= TLB_ENTRY_NUMBER)
        current_TLB =0;
    index_input = current_TLB;
    //my_printk("miss(%d)\n",global_i++);
    for(i=0;i<PTE_NUMBER;i++)
    {
        if(((PTE_info[i].virtue & 0xffffe000) == target) && ((PTE_info[i].usage == 0) || (PTE_info[i].asid == asid_input)))
        {
            //my_printk("PTE:%d  current_id:%d!\n",PTE_info[i].asid,asid_input);
            //my_printk("loading%d! virtue:%x index:%x\n",i,PTE_info[i].virtue,index_input);
            //my_printk("load a new page(%d)",global_i++);
            int change = TLB_info[index_input].PTE_number;
            if( change != -1)
            {
                //my_printk("exit TLB\n");
                PTE_info[change].inTLB = 0;
                PTE_info[change + 1].inTLB = 0;
            } 
            TLB_info[index_input].PTE_number = i;

            PTE_info[i].asid = asid_input;
            PTE_info[i+1].asid = asid_input;
            PTE_info[i].usage = 1;
            PTE_info[i+1].usage = 1;
            PTE_info[i].inTLB = 1;
            PTE_info[i+1].inTLB = 1;
            TLB_load(i,i+1,asid_input,index_input);
            symbol = 1;
            break;
        }
    }
    if(symbol == 0)
    {
        //my_printk("same virtue address!!!!!!!!!!!!\n");
        
        int new = current_virtue_ad;

        int change = TLB_info[index_input].PTE_number;
        if( change != -1)
        {
            //my_printk("exit TLB\n");
            PTE_info[change].inTLB = 0;
            PTE_info[change + 1].inTLB = 0;
        } 
        TLB_info[index_input].PTE_number = new;

        PTE_info[current_virtue_ad++].virtue = target;
        PTE_info[current_virtue_ad++].virtue = target;
        PTE_info[new].asid = asid_input;
        PTE_info[new+1].asid = asid_input;
        PTE_info[new].usage = 1;
        PTE_info[new+1].usage = 1;
        PTE_info[new].inTLB = 1;
        PTE_info[new+1].inTLB = 1;
        TLB_load(new,new+1,asid_input,index_input);
    }
    current_TLB++;
}

void TLB_invalid(int context,int index)
{
    //my_printk("invalid!");
    int i;
    //my_printk("context:%x!",context);
    int target = context & 0xffffe000;
    //printk("target:%x!",target);
    int index_input = index & 0x0000003f;
    int asid_input = current_running->pid;
    //my_printk("id:%d!\n",asid_input);
    //my_printk("PTE0:%d!\n",PTE_info[0].asid);
    //my_printk("invalid(%d)",global_j++);
    for(i=0;i<PTE_NUMBER;i++)
    {
        if(((PTE_info[i].virtue & 0xffffe000) == target) && (PTE_info[i].asid == asid_input))
        {
            if(PTE_info[i].onsd == 1)
            {
                //my_printk("swap in:%d!\n",i);
                page_swap_in(i,asid_input,index_input);
                break;
            }
            if(current_real_ad <MAX_REAL)
            {
                PTE_info[i].real = 0x00080000 + current_real_ad*1024*4;
                //PTE_info[i].usage =1;
                PTE_info[i].valid =1;
                current_real_ad++;
                PTE_info[i+1].real = 0x00080000 + current_real_ad*1024*4;
                //PTE_info[i+1].usage =1;
                PTE_info[i+1].valid =1;
                current_real_ad++;
                TLB_load(i,i+1,asid_input,index_input);
                break;
            }
            else
            {
                page_swap_out(i,asid_input,index_input);
                break;
            }
                
        }
    }
}

void TLB_helper(int context,int index,int badaddr)
{
    //my_printk("badaddr:%x;context:%x",badaddr);
    //int context_input = context << 9;
    if((index & 0x80000000) == 0)
        TLB_invalid(badaddr,index);
    else
        TLB_miss(badaddr);

}

void page_swap_out(int number,int asid_input,int index_input)
{
    while((PTE_info[swap_target].inTLB == 1) || (PTE_info[swap_target].valid == 0))
    {
        swap_target++;
        swap_target++;
        if(swap_target >= PTE_NUMBER)
            swap_target = 0;
    }
    int target = swap_target++;
    swap_target++;
    if(swap_target >= PTE_NUMBER)
        swap_target = 0;
    //my_printk("swap out:%d for %d!\n",target,number);

    //PTE_info[target].valid = 1;
    //PTE_info[target+1].valid = 1;
    TLB_load(target,target+1,asid_input,31);

    PTE_info[target].sdaddr = sd_target;
    sdwrite(PTE_info[target].virtue,sd_target,4*1024);
    sd_target += 4*1024;
    PTE_info[target].valid = 0;
    PTE_info[target].onsd = 1;
    
    //my_printk("finish 50!\n");
    PTE_info[target+1].sdaddr = sd_target;
    sdwrite(PTE_info[target+1].virtue,sd_target,4*1024);
    sd_target += 4*1024;
    PTE_info[target+1].valid = 0;
    PTE_info[target+1].onsd = 1;

    PTE_info[number].real = PTE_info[target].real;
    PTE_info[number].valid =1;
    PTE_info[number+1].real = PTE_info[target+1].real;
    PTE_info[number+1].valid =1;
    TLB_load(number,number+1,asid_input,index_input);
    TLB_load(target,target+1,asid_input,31);
}

void page_swap_in(int number,int asid_input,int index_input)
{
    while((PTE_info[swap_target].inTLB == 1) || (PTE_info[swap_target].valid == 0))
    {
        swap_target++;
        swap_target++;
        if(swap_target >= PTE_NUMBER)
            swap_target = 0;
    }
    int target = swap_target++;
    swap_target++;
    if(swap_target >= PTE_NUMBER)
        swap_target = 0;
    //my_printk("swap in:%d to %d!\n",number,target);

    TLB_load(target,target+1,asid_input,31);

    PTE_info[target].sdaddr = sd_target;
    sdwrite(PTE_info[target].virtue,sd_target,4*1024);
    sd_target += 4*1024;
    PTE_info[target].valid = 0;
    PTE_info[target].onsd = 1;

    PTE_info[target+1].sdaddr = sd_target;
    sdwrite(PTE_info[target+1].virtue,sd_target,4*1024);
    sd_target += 4*1024;
    PTE_info[target+1].valid = 0;
    PTE_info[target+1].onsd = 1;

    PTE_info[number].real = PTE_info[target].real;
    PTE_info[number].valid =1;
    PTE_info[number+1].real = PTE_info[target+1].real;
    PTE_info[number+1].valid =1;
    TLB_load(number,number+1,asid_input,index_input);

    sdread(PTE_info[number].virtue,PTE_info[number].sdaddr,4*1024);
    sdread(PTE_info[number+1].virtue,PTE_info[number+1].sdaddr,4*1024);
    PTE_info[number].onsd =0;
    PTE_info[number+1].onsd =0;
    
    TLB_load(target,target+1,asid_input,31);
    //TLB_load(number,number+1,asid_input,index_input);
}