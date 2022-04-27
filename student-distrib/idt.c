/* idt.c - Function supporting the IDT
 * 
 */

#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "scheduling.h"
#include "syscall.h"

int32_t exception_happen;
/* void* exception_handler(uint32_t num);
 * Inputs:      uint32_t num = the number index of the exception
 * Return Value: none
 * Function: print the exception information to the console */
void exception_handler(uint32_t num) {
    // Use num to determine which exception
    switch (num)
    {
    case 0:
        exception_happen = 1;
        printf("Devide Error Exception\n");
        sys_halt(0);
        break;
    case 1:
        exception_happen = 1;
        printf("Debug Exception\n");
        sys_halt(0);
        break;
    case 2:
        exception_happen = 1;
        printf("NMI Interrupt\n");
        sys_halt(0);
        break;
    case 3:
        exception_happen = 1;
        printf("Breakpoint Exception\n");
        sys_halt(0);
        break;
    case 4:
        exception_happen = 1;
        printf("Overflow Exception\n");
        sys_halt(0);
        break;
    case 5:
        exception_happen = 1;
        printf("BOUND Range Exceeded Exception\n");
        sys_halt(0);
        break;
    case 6:
        exception_happen = 1;
        printf("Invalid Opcode Exception\n");
        sys_halt(0);
        break;
    case 7:
        exception_happen = 1;
        printf("Device Not Available Exception\n");
        sys_halt(0);
        break;
    case 8:
        exception_happen = 1;
        printf("Double Fault Exception\n");
        sys_halt(0);
        break;
    case 9:
        exception_happen = 1;
        printf("Coprocessor Segment Overrun\n");
        sys_halt(0);
        break;
    case 10:
        exception_happen = 1;
        printf("Invalid TSS Exception\n");
        sys_halt(0);
        break;
    case 11:
        exception_happen = 1;
        printf("Segment Not Present\n");
        sys_halt(0);
        break;
    case 12:
        exception_happen = 1;
        printf("Stack Fault Exception\n");
        sys_halt(0);
        break;
    case 13:
        exception_happen = 1;
        printf("General Protection Exception\n");
        sys_halt(0);
        break;
    case 14:
        exception_happen = 1;
        printf("Page-Fault Exception\n");
        sys_halt(0);
        break;
    case 16:
        exception_happen = 1;
        printf("x87 FPU Floating-Point Error\n");
        sys_halt(0);
        break;
    case 17:
        exception_happen = 1;
        printf("Alignment Check Exception\n");

        sys_halt(0);
        break;
    case 18:
        exception_happen = 1;
        printf("Machine-Check Exception\n");
        sys_halt(0);
        break;
    case 19:
        exception_happen = 1;
        printf("SIMD Floating-Point Exception\n");
        sys_halt(0);
        break;
    default:
        break;
    }
}

/* void* interrupt_handler(uint32_t num);
 * Inputs:      uint32_t num = the number index of the interrupt
 * Return Value: none
 * Function: handle the proper interrupt */
void interrupt_handler(uint32_t num)
{
    switch (num)
    {
        case 0:
            PIT_handler();
            break;
        case 1:
            keyboard_handler();
            break;
        case 8:
            RTC_handler();
            break;
        default:
            break;
    }
    return;
}

/* void* idt_initial(void);
 * Inputs:      none
 * Return Value: none
 * Function: Initial the IDT */
void idt_initial(void) {
    int i;  /* used as the number of the for loop.      */

    // Set the function pointer of the IDT
    SET_IDT_ENTRY(idt[0], DEVIDE_ERROR_EXCEPTION);
    SET_IDT_ENTRY(idt[1], DEBUG_EXCEPTION);
    SET_IDT_ENTRY(idt[2], NMI_INTERRUPT);
    SET_IDT_ENTRY(idt[3], BREAKPOINT_EXCEPTION);
    SET_IDT_ENTRY(idt[4], OVERFLOW_EXCEPTION);
    SET_IDT_ENTRY(idt[5], BOUND_RANGE_EXCEEDED_EXCEPTION);
    SET_IDT_ENTRY(idt[6], INVALID_OPCODE_EXCEPTION);
    SET_IDT_ENTRY(idt[7], DEVICE_NOT_AVAILABLE_EXCEPTION);
    SET_IDT_ENTRY(idt[8], DOUBLE_FAULT_EXCEPTION);
    SET_IDT_ENTRY(idt[9], COPROCESSOR_SEGMENT_OVERRUN);
    SET_IDT_ENTRY(idt[10], INVALID_TSS_EXCEPTION);
    SET_IDT_ENTRY(idt[11], SEGMENT_NOT_PRESENT);
    SET_IDT_ENTRY(idt[12], STACK_FAULT_EXCEPTION);
    SET_IDT_ENTRY(idt[13], GENERAL_PROTECTION_EXCEPTION);
    SET_IDT_ENTRY(idt[14], PAGE_FAULT_EXCEPTION);
    SET_IDT_ENTRY(idt[16], MATH_ERROR);
    SET_IDT_ENTRY(idt[17], ALIGNMENT_CHECK_EXCEPTION);
    SET_IDT_ENTRY(idt[18], MACHINE_CHECK_EXCEPTION);
    SET_IDT_ENTRY(idt[19], FLOATING_POINT_EXCEPTION);
    SET_IDT_ENTRY(idt[PIT_IDT], PIT_INT);
    SET_IDT_ENTRY(idt[KEYBOARD_IDT], KEYBOARD_INT);
    SET_IDT_ENTRY(idt[RTC_IDT], RTC_INT);
    SET_IDT_ENTRY(idt[SYSTEM_IDT], system_call);

    for (i = 0; i < EXCEPTION_NUM; i++) {                            /* We first set all the exceptions to be present.*/
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 1;
    }
    for (i = EXCEPTION_NUM; i < NUM_VEC; i++){                       /* Then we set all the other IDT entries to be not present.*/
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        idt[i].present = 0;
    }
    idt[KEYBOARD_IDT].reserved3 = 0;
    idt[RTC_IDT].reserved3 = 0;
    idt[KEYBOARD_IDT].present = 1;
    idt[RTC_IDT].present = 1;
    idt[SYSTEM_IDT].present = 1;
    idt[SYSTEM_IDT].dpl = 3;
    idt[PIT_IDT].reserved3 = 0; // change from TRAP to INTR
    idt[PIT_IDT].present = 1;
}
