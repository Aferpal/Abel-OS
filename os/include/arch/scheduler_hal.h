#ifndef _SCHEDULER_HAL_H_
#define _SCHEDULER_HAL_H_

void* arch_proc_prepare_stack(void (*p_start)(void), void*);
void* arch_proc_prepare_user_stack(void*, ptr_t code_start, ptr_t user_stack);

#endif
