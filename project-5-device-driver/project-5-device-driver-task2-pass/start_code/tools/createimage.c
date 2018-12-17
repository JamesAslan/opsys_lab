#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_bootblock(FILE *image, FILE *bbfile, Elf32_Phdr *Phdr);
Elf32_Phdr *read_exec_file(FILE *opfile);
uint32_t count_kernel_sectors(Elf32_Phdr *Phdr);
void extent_opt(Elf32_Phdr *Phdr_bb, Elf32_Phdr *Phdr_k, int kernelsz);


Elf32_Phdr *read_exec_file(FILE *opfile)
{
    //struct Elf32_Phdr a;
    char middleground[4096];
    fread(middleground, 1, 4096, opfile);
    Elf32_Phdr *Phdr = (Elf32_Phdr *)calloc(1,sizeof(Elf32_Phdr));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    Elf32_Ehdr *ELF_head = (Elf32_Ehdr *)middleground;
    memcpy(Phdr, (void *)(middleground + (*ELF_head).e_phoff), sizeof(Elf32_Phdr));

    return Phdr;
}

uint32_t count_kernel_sectors(Elf32_Phdr *Phdr)
{
    uint32_t result = ((*Phdr).p_memsz+511 ) / 512;
    return result;
}

void write_bootblock(FILE *image, FILE *file, Elf32_Phdr *phdr)
{
    int size = (*phdr).p_memsz;
    int offset = (*phdr).p_offset;
    fseek (file,offset,SEEK_SET);

    char middleground[4096] ;

    fread (middleground,1,size,file);
    fwrite (middleground,1,size,image);

}

void write_kernel(FILE *image, FILE *knfile, Elf32_Phdr *Phdr, int kernelsz)
{
    int size = (*Phdr).p_offset;
    int offset = 512;
    char middleground[2048];
    unsigned int memsize = (*Phdr).p_memsz;
    unsigned int filesize = (*Phdr).p_filesz;
    fseek(knfile, size, SEEK_SET);
    fseek(image, offset, SEEK_SET);
    while(filesize > 0)
    {
        fread (middleground,1,1,knfile);
        fwrite (middleground,1,1,image);
        filesize -- ;
    }
    
    char zero_content = 0;
    char *zero_point = &zero_content;
    int rich = (*Phdr).p_memsz - (*Phdr).p_filesz;
    while(rich > 0)
    {
        fwrite (zero_point,1,1,image);
        rich -- ;
    }
}

void record_kernel_sectors(FILE *image, uint32_t kernelsz)
{
    fseek(image, 511, SEEK_SET);
    fwrite (&kernelsz,1,1,image);
}


void extent_opt(Elf32_Phdr *Phdr_bb, Elf32_Phdr *Phdr_k, int kernelsz)
{
    printf("The size of bootblock in file is %d\n",Phdr_bb->p_filesz);
    printf("The size of bootblock in memory is %d\n",Phdr_bb->p_memsz);
    printf("The size of kernel is %d\n",kernelsz);
    printf("\n");

    printf("The offset of bootblock in memory is %d\n",Phdr_bb->p_offset);
    printf("The size of kernel in memory is %d\n",Phdr_k->p_memsz);
    printf("The size of kernel in file is %d\n",Phdr_k->p_filesz);
    printf("The offset of kernel in memory is %d\n",Phdr_k->p_offset);

}

int main()
{
    FILE *opfile_k= fopen("main","rb");
    FILE *opfile_bb= fopen("bootblock","rb");
    FILE *opfile_i= fopen("image","wb"); 

    Elf32_Phdr *Phdr_bb= read_exec_file(opfile_bb);
    Elf32_Phdr *Phdr_k= read_exec_file(opfile_k);
    uint32_t kernelsz = count_kernel_sectors(Phdr_k);

    write_bootblock(opfile_i, opfile_bb, Phdr_bb);
    write_kernel(opfile_i, opfile_k, Phdr_k, kernelsz);
    record_kernel_sectors(opfile_i,kernelsz);

    extent_opt(Phdr_bb, Phdr_k, kernelsz);

    fclose(opfile_k);
    fclose(opfile_bb);
    fclose(opfile_i);

}
