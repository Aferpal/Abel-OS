#ifndef _ARCH_SYSCALL_H_
#define _ARCH_SYSCALL_H_
#include "stdint.h"

typedef int32_t(*syscall_dispatcher_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void arch_syscall_init(syscall_dispatcher_t i_syscall_dispatch);

#endif