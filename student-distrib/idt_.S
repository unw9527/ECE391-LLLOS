.globl DEVIDE_ERROR_EXCEPTION, DEBUG_EXCEPTION, NMI_INTERRUPT, BREAKPOINT_EXCEPTION, OVERFLOW_EXCEPTION
.globl BOUND_RANGE_EXCEEDED_EXCEPTION, INVALID_OPCODE_EXCEPTION, DEVICE_NOT_AVAILABLE_EXCEPTION
.globl DOUBLE_FAULT_EXCEPTION, COPROCESSOR_SEGMENT_OVERRUN, INVALID_TSS_EXCEPTION, SEGMENT_NOT_PRESENT
.globl STACK_FAULT_EXCEPTION, GENERAL_PROTECTION_EXCEPTION, PAGE_FAULT_EXCEPTION, MATH_ERROR
.globl ALIGNMENT_CHECK_EXCEPTION, MACHINE_CHECK_EXCEPTION, FLOATING_POINT_EXCEPTION
.globl KEYBOARD_INT, RTC_INT, PIT_INT


# The exception jump table
DEVIDE_ERROR_EXCEPTION:
    cli
    pushl $0                                                          # Push the EXCP value onto the stack.
    jmp COMMON_EXCP                                                  # Jump to the common exception.
DEBUG_EXCEPTION:
    cli
    pushl $1
    jmp COMMON_EXCP
NMI_INTERRUPT:
    cli
    pushl $2
    jmp COMMON_EXCP
BREAKPOINT_EXCEPTION:
    cli
    pushl $3
    jmp COMMON_EXCP
OVERFLOW_EXCEPTION:
    cli
    pushl $4
    jmp COMMON_EXCP
BOUND_RANGE_EXCEEDED_EXCEPTION:
    cli
    pushl $5
    jmp COMMON_EXCP
INVALID_OPCODE_EXCEPTION:
    cli
    pushl $6
    jmp COMMON_EXCP
DEVICE_NOT_AVAILABLE_EXCEPTION:
    cli
    pushl $7
    jmp COMMON_EXCP
DOUBLE_FAULT_EXCEPTION:
    cli
    pushl $8
    jmp COMMON_EXCP
COPROCESSOR_SEGMENT_OVERRUN:
    cli
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
    pushl $16
    jmp COMMON_EXCP
ALIGNMENT_CHECK_EXCEPTION:
    cli
    pushl $17
    jmp COMMON_EXCP
MACHINE_CHECK_EXCEPTION:
    cli
    pushl $18
    jmp COMMON_EXCP
FLOATING_POINT_EXCEPTION:
    cli
    pushl $19
    jmp COMMON_EXCP




PIT_INT:
    cli
    pushl $0xFFFFFFFF
    jmp   COMMON_INT

KEYBOARD_INT:
    cli
    pushl $0xFFFFFFFE                                                 # Push the opposite value to the stack.
    jmp COMMON_INT                                                   # Jump to common interrupt.
RTC_INT:
    cli
    pushl $0xFFFFFFF7
    jmp COMMON_INT


# Exception handler
COMMON_EXCP:                                                         # This is the common exception code.
    pushl %eax
    movl 4(%esp), %eax                                               # Let eax stores the value of INT vector.
    pushal                                                           # Push all the registers.
    pushfl
    pushl %eax                                                       # Push the INT vector onto the stack.
    call exception_handler
    popl %eax
    popfl
    popal
    popl %eax
    addl $4, %esp
    sti
    iret

# Interrupt handler
COMMON_INT:
    pushl %eax
    movl 4(%esp), %eax
    pushal
    pushfl
    cmpl $0, %eax
    jl do_INT

    # This position is for system call.

do_INT:
    xorl $0xFFFFFFFF, %eax
    pushl %eax
    call interrupt_handler
    popl %eax
    popfl
    popal
    popl %eax
    addl $4, %esp
    sti
    iret
