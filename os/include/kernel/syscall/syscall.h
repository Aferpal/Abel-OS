#ifndef _SYSCALL_H_
#define _SYSCALL_H_

typedef int32_t(*syscall_handler_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

void init_syscall_subsystem();

#endif