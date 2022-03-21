#ifndef _IDT_H
#define _IDT_H
#include "types.h"

#define EXCEPTION_NUM 20

void idt_initial(void);

void exception_handler(uint32_t num);

void interrupt_handler(uint32_t num);

void DEVIDE_ERROR_EXCEPTION(void);

void DEBUG_EXCEPTION(void);

void NMI_INTERRUPT(void);

void BREAKPOINT_EXCEPTION(void);

void OVERFLOW_EXCEPTION(void);

void BOUND_RANGE_EXCEEDED_EXCEPTION(void);

void INVALID_OPCODE_EXCEPTION(void);

void DEVICE_NOT_AVAILABLE_EXCEPTION(void);

void DOUBLE_FAULT_EXCEPTION(void);

void COPROCESSOR_SEGMENT_OVERRUN(void);

void INVALID_TSS_EXCEPTION(void);

void SEGMENT_NOT_PRESENT(void);

void STACK_FAULT_EXCEPTION(void);

void GENERAL_PROTECTION_EXCEPTION(void);

void PAGE_FAULT_EXCEPTION(void);

void MATH_ERROR(void);

void ALIGNMENT_CHECK_EXCEPTION(void);

void MACHINE_CHECK_EXCEPTION(void);

void FLOATING_POINT_EXCEPTION(void);

void KEYBOARD_INT(void);

void RTC_INT(void);

#endif
