#ifndef _SYSCALL_DEFINES_H_
#define _SYSCALL_DEFINES_H_

#define SYSCALL_FROM_NAME(name) sys_##name

#define SYSCALL_DEFINE(name) \
    static int32_t SYSCALL_FROM_NAME(name)(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)

typedef enum syscall_return_type {
    ENOTIM = 1,
    EINVAL = 2,
} syscall_return_type_e;

#endif