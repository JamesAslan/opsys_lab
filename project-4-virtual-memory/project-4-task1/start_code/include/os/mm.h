#ifndef INCLUDE_MM_H_
#define INCLUDE_MM_H_

#include "type.h"

#define TLB_ENTRY_NUMBER 64
#define PTE_NUMBER 8000

struct PTE{
    int id;
    int usage;
    int virtue;
    int real;
    int valid;
    int dirty;
    int global;
    int coherency;  
};

struct TLB_info{
    int valid;
    int usage;
};

int             real_address_available[8000];
struct PTE      PTE_info[PTE_NUMBER];
struct TLB_info TLB_info[TLB_ENTRY_NUMBER];
int             current_TLB;

void init_page_table(void);
void TLB_info_clear(void);
void PTE_info_clear(void);

void TLB_load(int , int);


void do_TLB_Refill();
void do_page_fault();

#endif
