#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_

#include "type.h"

#define TLB_ENTRY_NUMBER 12
#define PTE_NUMBER 80000
#define PTE_RICH 60000
#define MAX_REAL 30

struct PTE{
    int asid;
    int usage;
    int virtue;
    int real;
    int valid;
    int dirty;
    int global;
    int coherency;
    int onsd;  
    int sdaddr;
    int inTLB;
};

struct TLB_info{
    int PTE_number;
};

int             real_address_available[8000];
struct PTE      PTE_info[PTE_NUMBER];
struct TLB_info TLB_info[TLB_ENTRY_NUMBER];
int             current_TLB;
int             current_real_ad;
int             current_virtue_ad;

int             swap_target;
int             sd_target;

void init_page_table(void);
void TLB_info_clear(void);
void PTE_info_clear(void);

void TLB_load(int , int,int ,int);

void page_swap_out(int, int, int);
void page_swap_in(int, int, int);

void do_TLB_Refill();
void do_page_fault();

void TLB_helper(int, int,int);

#endif
