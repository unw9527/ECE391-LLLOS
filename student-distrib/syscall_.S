.globl jump_table_rtc, jump_table_file, jump_table_dir, jump_table_terminal, system_call

.align 4

jump_table_rtc:
    .long open_rtc_pt
    .long read_rtc_pt
    .long close_rtc_pt
    .long write_rtc_pt
jump_table_file:
    .long open_file_pt
    .long read_file_pt
    .long close_file_pt
    .long write_file_pt

jump_table_dir:
    .long open_dir_pt
    .long read_dir_pt
    .long close_dir_pt
    .long write_dir_pt

jump_table_terminal:
    .long open_terminal_pt
    .long read_terminal_pt
    .long close_terminal_pt
    .long write_terminal_pt


.text

jump_table_syscall:
    # Use a padding.
    .long 0, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

system_call:
    # Save all the registers in the correct order.
    pushl $0
    pushl %eax
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
    # Check whether the system call number is valid.
    cmpl $1, %eax
    jl fail
    cmpl $10, %eax
    jg fail
    movw $0x18, %bx
    movw %bx, %ds
    # Call the sub function through jump table.
    call *jump_table_syscall(, %eax, 4)
    jmp return_system_call
fail:
    # Return -1 in this case.
    movl $-1, %eax
return_system_call:
    cmpl $0x999, %eax
    je POP_REG1
    cmpl $0x888, %eax
    je POP_REG2
    movl %eax, 24(%esp)
    movl 52(%esp), %eax
    pushl %eax
    call check_pending
    popl %ebx
    cmpl $-1, %eax
    jne SIGNAL_DEAL
    # Pop all the registers in the correct order.
POP_REG1:
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
    iret
    # Do not need to modify the %eax value since this is already done in the sub function.

    # The case for user level.
POP_REG2:
    addl $8, %esp
    jmp POP_REG1
.end
