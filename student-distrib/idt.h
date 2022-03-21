/* idt.h - Defines for idt.c and idt_.S
 * 
 */
#ifndef _IDT_H
#define _IDT_H
#include "types.h"

#define EXCEPTION_NUM           20          // The number of the exception
#define KEYBOARD_IDT            0x21        // The IDT entry of keyboard
#define RTC_IDT                 0x28        // The IDT entry of RTC

/* The initial function of the IDT.         */
void idt_initial(void);

/* The function to handle the exception.    */
void exception_handler(uint32_t num);

/* The function to handle the interrupt.    */
void interrupt_handler(uint32_t num);

/* -------------------- Exception Function -------------------- */
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
/* -------------------------------------------------------------- */

/* The initial function of keyboard.    */
void KEYBOARD_INT(void);

/* The initial function of RTC.         */
void RTC_INT(void);

#endif
