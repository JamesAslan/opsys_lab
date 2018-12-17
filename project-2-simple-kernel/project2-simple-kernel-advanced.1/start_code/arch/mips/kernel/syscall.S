#include "asm.h"

LEAF(invoke_syscall)
    // TODO syscall entry
    syscall
END(invoke_syscall)