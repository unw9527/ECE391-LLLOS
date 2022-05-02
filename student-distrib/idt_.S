.globl DEVIDE_ERROR_EXCEPTION, DEBUG_EXCEPTION, NMI_INTERRUPT, BREAKPOINT_EXCEPTION, OVERFLOW_EXCEPTION
.globl BOUND_RANGE_EXCEEDED_EXCEPTION, INVALID_OPCODE_EXCEPTION, DEVICE_NOT_AVAILABLE_EXCEPTION
.globl DOUBLE_FAULT_EXCEPTION, COPROCESSOR_SEGMENT_OVERRUN, INVALID_TSS_EXCEPTION, SEGMENT_NOT_PRESENT
.globl STACK_FAULT_EXCEPTION, GENERAL_PROTECTION_EXCEPTION, PAGE_FAULT_EXCEPTION, MATH_ERROR
.globl ALIGNMENT_CHECK_EXCEPTION, MACHINE_CHECK_EXCEPTION, FLOATING_POINT_EXCEPTION
.globl KEYBOARD_INT, RTC_INT, PIT_INT, MOUSE_INT, SIGNAL_DEAL, SIGNAL_LINKAGE

DEVIDE_ERROR_EXCEPTION:
    cli
    pushl $0
    pushl $0                                                          # Push the EXCP value onto the stack.
    jmp COMMON_EXCP                                                  # Jump to the common exception.
DEBUG_EXCEPTION:
    cli
    pushl $0
    pushl $1
    jmp COMMON_EXCP
NMI_INTERRUPT:
    cli
    pushl $0
    pushl $2
    jmp COMMON_EXCP
BREAKPOINT_EXCEPTION:
    cli
    pushl $0
    pushl $3
    jmp COMMON_EXCP
OVERFLOW_EXCEPTION:
    cli
    pushl $0
    pushl $4
    jmp COMMON_EXCP
BOUND_RANGE_EXCEEDED_EXCEPTION:
    cli
    pushl $0
    pushl $5
    jmp COMMON_EXCP
INVALID_OPCODE_EXCEPTION:
    cli
    pushl $0
    pushl $6
    jmp COMMON_EXCP
DEVICE_NOT_AVAILABLE_EXCEPTION:
    cli
    pushl $0
    pushl $7
    jmp COMMON_EXCP
DOUBLE_FAULT_EXCEPTION:
    cli
    pushl $8
    jmp COMMON_EXCP
COPROCESSOR_SEGMENT_OVERRUN:
    cli
    pushl $0
    pushl $9
    jmp COMMON_EXCP
INVALID_TSS_EXCEPTION:
    cli
    pushl $10
    jmp COMMON_EXCP
SEGMENT_NOT_PRESENT:
    cli
    pushl $11
    jmp COMMON_EXCP
STACK_FAULT_EXCEPTION:
    cli
    pushl $12
    jmp COMMON_EXCP
GENERAL_PROTECTION_EXCEPTION:
    cli
    pushl $13
    jmp COMMON_EXCP
PAGE_FAULT_EXCEPTION:
    cli
    pushl $14
    jmp COMMON_EXCP
MATH_ERROR:
    cli
    pushl $0
    pushl $16
    jmp COMMON_EXCP
ALIGNMENT_CHECK_EXCEPTION:
    cli
    pushl $17
    jmp COMMON_EXCP
MACHINE_CHECK_EXCEPTION:
    cli
    pushl $0
    pushl $18
    jmp COMMON_EXCP
FLOATING_POINT_EXCEPTION:
    cli
    pushl $0
    pushl $19
    jmp COMMON_EXCP




PIT_INT:
    cli
    pushl $0
    pushl $0xFFFFFFFF
    jmp   COMMON_INT

KEYBOARD_INT:
    cli
    pushl $0
    pushl $0xFFFFFFFE                                                 # Push the opposite value to the stack.
    jmp COMMON_INT                                                   # Jump to common interrupt.
RTC_INT:
    cli
    pushl $0
    pushl $0xFFFFFFF7
    jmp COMMON_INT

MOUSE_INT:
    cli
    pushl $0
    pushl $0xFFFFFFF3
    jmp COMMON_INT

COMMON_EXCP:                                                         # This is the common exception code.
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    movl 40(%esp), %eax
    pushl %eax
    call exception_handler
    popl %eax
    movl 52(%esp), %eax
    pushl %eax
    call check_pending
    popl %ebx
    cmpl $-1, %eax
    jne SIGNAL_DEAL
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $8, %esp
    sti
    iret

COMMON_INT:
    pushl %fs
    pushl %es
    pushl %ds
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    movl 40(%esp), %eax
    xorl $0xFFFFFFFF, %eax
    pushl %eax
    call interrupt_handler
    popl %eax
    movl 52(%esp), %eax
    pushl %eax
    call check_pending
    popl %ebx
    cmpl $-1, %eax
    jne SIGNAL_DEAL
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %eax
    popl %ds
    popl %es
    popl %fs
    addl $8, %esp
    sti
    iret

SIGNAL_DEAL:
    pushl %eax
    call user_signal

SIGNAL_LINKAGE:
    addl $4, %esp
    movl $10, %eax
    int $0x80

