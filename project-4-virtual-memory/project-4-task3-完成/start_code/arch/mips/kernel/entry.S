#include "asm.h"

.equ    KERNEL, 0
.equ    USER,   156

.macro STI
.endm

.macro CLI
.endm

.global initial_helper

LEAF(asm_start)
    mtc0    zero, CP0_STATUS
    mtc0    zero, CP0_WATCHLO
    mtc0    zero, CP0_WATCHHI

    mfc0    t0, CP0_CONFIG
    and     t0, ~0x7
    ori     t0, 0x2
    mtc0    t0, CP0_CONFIG
    
    jr  ra
END(asm_start)

LEAF(get_cp0_status)
    mfc0 v0, CP0_STATUS

    jr  ra
END(get_cp0_status)

LEAF(set_cp0_status)
    mtc0 a0, CP0_STATUS

    jr  ra
END(set_cp0_status)

LEAF(cp0_init)
    mtc0    zero, CP0_STATUS
    li      k0, 1500000
    mtc0    k0,  CP0_COMPARE
    mtc0    zero, CP0_COUNT

    jr ra
END(cp0_init)

LEAF(cp0_termi)
    mtc0   zero, CP0_COUNT
    li     k0, 0x00008001
    mtc0   k0, CP0_STATUS

    jr ra
END(cp0_termi)

LEAF(cp0_count_remove)
    li      k0, 1500000
    mtc0    k0,  CP0_COMPARE
    mtc0    zero, CP0_COUNT

    jr ra
END(cp0_count_remove)

LEAF(TLB_info_load)
    mtc0    a0, CP0_ENTRYHI
    mtc0    a1, CP0_ENTRYLO0
    mtc0    a2, CP0_ENTRYLO1
    li      k1, 0
    mtc0    k1, CP0_PAGEMASK
    mtc0    a3, CP0_INDEX
    tlbwi
    jr ra
END(TLB_info_load)

LEAF(load_entryhi)
    mtc0    a0, CP0_ENTRYHI
    jr ra
END(load_entryhi)

.macro SAVE_CONTEXT offset
    // TODO save context
    lw k0, current_running
    li k1, \offset
    add k0, k0, k1
    #sw $0, 0(k0)
    #sw $1, 4(k0)
    sw $2, 8(k0)
    sw $3, 12(k0)
    sw $4, 16(k0)
    sw $5, 20(k0)
    sw $6, 24(k0)
    sw $7, 28(k0)
    sw $8, 32(k0)
    sw $9, 36(k0)
    sw $10, 40(k0)
    sw $11, 44(k0)
    sw $12, 48(k0)
    sw $13, 52(k0)
    sw $14, 56(k0)
    sw $15, 60(k0)
    sw $16, 64(k0)
    sw $17, 68(k0)
    sw $18, 72(k0)
    sw $19, 76(k0)
    sw $20, 80(k0)
    sw $21, 84(k0)
    sw $22, 88(k0)
    sw $23, 92(k0)
    sw $24, 96(k0)
    sw $25, 100(k0)
    #sw $26, 104(k0)
    #sw $27, 108(k0)
    #sw $28, 112(k0)
    sw $29, 116(k0)
    sw $30, 120(k0)
    sw $31, 124(k0)
    mfc0 t0, CP0_STATUS
    sw t0, 128(k0)
    mfhi t0
    sw t0, 132(k0)
    mflo t0
    sw t0, 136(k0)
    mfc0 t0, CP0_BADVADDR
    sw t0, 140(k0)
    mfc0 t0, CP0_CAUSE
    sw t0, 144(k0)
    mfc0 t0, CP0_EPC
    sw t0, 148(k0)
    #pc
.endm

.macro RESTORE_CONTEXT offset
    // TODO restore context
    lw k0, current_running
    li k1, \offset
    add k0, k0, k1
    #lw $0, 0(k0)
    #lw $1, 4(k0)
    lw $2, 8(k0) 
    lw $3, 12(k0)
    lw $4, 16(k0) 
    lw $5, 20(k0) 
    lw $6, 24(k0)
    lw $7, 28(k0)
    lw $8, 32(k0)
    lw $9, 36(k0)
    lw $10, 40(k0)
    lw $11, 44(k0)
    lw $12, 48(k0)
    lw $13, 52(k0) 
    lw $14, 56(k0)
    lw $15, 60(k0)
    lw $16, 64(k0) 
    lw $17, 68(k0) 
    lw $18, 72(k0) 
    lw $19, 76(k0) 
    lw $20, 80(k0) 
    lw $21, 84(k0)
    lw $22, 88(k0) 
    lw $23, 92(k0) 
    lw $24, 96(k0) 
    lw $25, 100(k0)
    #lw $26, 104(k0)
    #lw $27, 108(k0)
    #lw $28, 112(k0) 
    lw $29, 116(k0) 
    lw $30, 120(k0) 
    lw $31, 124(k0)
    lw t0, 128(k0) 
    mtc0 t0, CP0_STATUS
    lw t0, 132(k0)
    mthi t0
    lw t0, 136(k0)
    mtlo t0
    lw t0, 140(k0)
    mtc0 t0, CP0_BADVADDR
    lw t0, 144(k0)
    mtc0 t0, CP0_CAUSE
    lw t0, 148(k0) 
    mtc0 t0, CP0_EPC
    #pc
.endm

.macro RESTORE_CONTEXT_SYSCALL offset
    // TODO restore context
    lw k0, current_running
    li k1, \offset
    add k0, k0, k1
    #lw $0, 0(k0)
    #lw $1, 4(k0)
    #lw $2, 8(k0) 
    #lw $3, 12(k0)
    lw $4, 16(k0) 
    lw $5, 20(k0) 
    lw $6, 24(k0)
    lw $7, 28(k0)
    lw $8, 32(k0)
    lw $9, 36(k0)
    lw $10, 40(k0)
    lw $11, 44(k0)
    lw $12, 48(k0)
    lw $13, 52(k0) 
    lw $14, 56(k0)
    lw $15, 60(k0)
    lw $16, 64(k0) 
    lw $17, 68(k0) 
    lw $18, 72(k0) 
    lw $19, 76(k0) 
    lw $20, 80(k0) 
    lw $21, 84(k0)
    lw $22, 88(k0) 
    lw $23, 92(k0) 
    lw $24, 96(k0) 
    lw $25, 100(k0)
    #lw $26, 104(k0)
    #lw $27, 108(k0)
    #lw $28, 112(k0) 
    lw $29, 116(k0) 
    lw $30, 120(k0) 
    lw $31, 124(k0)
    lw t0, 128(k0) 
    mtc0 t0, CP0_STATUS
    lw t0, 132(k0)
    mthi t0
    lw t0, 136(k0)
    mtlo t0
    lw t0, 140(k0)
    mtc0 t0, CP0_BADVADDR
    lw t0, 144(k0)
    mtc0 t0, CP0_CAUSE
    lw t0, 148(k0) 
    mtc0 t0, CP0_EPC
.endm

NESTED(do_scheduler, 0, ra)
    SAVE_CONTEXT(KERNEL)

    jal     scheduler
    RESTORE_CONTEXT(KERNEL)
    jr      ra
END(do_scheduler)

NESTED(get_addr, 0, ra)
    la  v0, initial_helper
    jr ra
END(get_addr)

.global exception_handler_begin
.global exception_handler_end
.global initial_helper
.global TLBexception_handler_begin
.global TLBexception_handler_end

NESTED(TLBexception_handler_entry, 0, sp)
TLBexception_handler_begin:
	//TODO: TLB exception entry
    // TODO close interrupt
    mfc0   k0, CP0_STATUS
    li     k1, 0xfffffffe
    and    k0, k0, k1
    mtc0   k0, CP0_STATUS

    SAVE_CONTEXT(USER)

    #lw      k0, current_running
    #lw      $29, 116(k0)

	//jmp exception_handler[i] which decided by CP0_CAUSE

    mfc0    k1, CP0_CAUSE
    li      k0, 0x000000ff
    and     k1, k1, k0

    addu    k0, zero, k1
    la      k1, exception_handler
    addu    k0, k0, k1
    lw      k1, 0(k0)
    jr      k1

TLBexception_handler_end:
END(TLBexception_handler_entry)

NESTED(exception_handler_entry, 0, sp)   
exception_handler_begin:
    // TODO close interrupt
    mfc0   k0, CP0_STATUS
    li     k1, 0xfffffffe
    and    k0, k0, k1
    mtc0   k0, CP0_STATUS

    SAVE_CONTEXT(USER)

    lw      k0, current_running
    lw      $29, 116(k0)
    #li      k0, 156
    #lw      $29, 116(k0)

    // jmp exception_handler[i] which decided by CP0_CAUSE
    mfc0    k1, CP0_CAUSE
    li      k0, 0x000000ff
    and     k1, k1, k0
    #srl     k1, k1, 2

    addu    k0, zero, k1
    la      k1, exception_handler
    addu    k0, k0, k1
    lw      k1, 0(k0)
    jr      k1

    // Leve2 exception Handler.
exception_handler_end:
END(exception_handler_entry)

NESTED(handle_int, 0, sp)
    // interrupt handler
    // Leve3 exception Handler.
    mfc0    k1, CP0_CAUSE
    move    a1, k1
    mfc0    k1, CP0_STATUS
    move    a0, k1
    addiu   sp,sp,-8
    jal     interrupt_helper
    addiu   sp,sp,8
       
    RESTORE_CONTEXT(USER)

    #j while1
    mfc0   k0, CP0_STATUS
    ori    k0, k0, 0x00008003
    mtc0   k0, CP0_STATUS

    eret
END(handle_int)

NESTED(first_handle_int, 0, sp)
initial_helper:

    li      k0, 1500000
    mtc0    k0,  CP0_COMPARE
    mtc0    zero, CP0_COUNT

    RESTORE_CONTEXT(USER)

    mfc0   k0, CP0_STATUS
    ori    k0, k0, 0x00008003
    mtc0   k0, CP0_STATUS

    eret
END(first_handle_int)

NESTED(handle_syscall, 0, sp)
    // system call handler
    
    lw     k0, current_running
    addi   k0 , k0 ,156
    lw     k1, 148(k0)
    addi   k1, k1 ,4 
    sw     k1, 148(k0)

    addiu   sp,sp,-8
    jal     system_call_helper
    addiu   sp,sp,8
    
    RESTORE_CONTEXT_SYSCALL(USER)
    
    mfc0   k0, CP0_STATUS
    ori    k0, k0, 0x00008003
    mtc0   k0, CP0_STATUS

    mfc0    k0, CP0_EPC

    eret

END(handle_syscall)

NESTED(handle_other, 0, sp)
    // other exception handler
END(handle_other)


NESTED(handle_TLB, 0, sp)
    // TLB handler
    // Leve3 exception Handler.
    mfc0    a0, CP0_CONTEXT
    mfc0    k0, CP0_BADVADDR
    li      k1, 0xffffe000
    and     a3, k0, k1
    mfc0    k0, CP0_ENTRYHI
    li      k1, 0x000000ff
    and     k0, k0, k1
    or      k0, k0, a3
    mtc0    k0, CP0_ENTRYHI
    tlbp
    mfc0    a1, CP0_INDEX
    mfc0    a2, CP0_BADVADDR

    addiu   sp,sp,-8
    jal     TLB_helper
    addiu   sp,sp,8

    #lw      k0, current_running
    #sw      $29, 116(k0)
       
    RESTORE_CONTEXT(USER)

    mfc0   k0, CP0_STATUS
    ori    k0, k0, 0x00008003
    mtc0   k0, CP0_STATUS

    mfc0   k0, CP0_EPC
    nop
    nop
    eret
END(handle_TLB)

#////////////////////////////
