#ifndef _EXCEPTION_HAL_H_
#define _EXCEPTION_HAL_H_

#include "kernel/interrupt/types.h"

int arch_vector_is_exception(struct interrupt_frame* frame);

void arch_exception_dispatch(struct interrupt_frame* frame);

#endif