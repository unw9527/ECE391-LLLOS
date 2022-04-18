.globl jump_table_rtc, jump_table_file, jump_table_dir, jump_table_terminal, system_call

.align 4


# The jump table of rtc file descriptor
jump_table_rtc:
    .long open_rtc_pt
    .long read_rtc_pt
    .long close_rtc_pt
    .long write_rtc_pt

# The jump table of file descriptor
jump_table_file:
    .long open_file_pt
    .long read_file_pt
    .long close_file_pt
    .long write_file_pt

# The jump table of file directory descriptor
jump_table_dir:
    .long open_dir_pt
    .long read_dir_pt
    .long close_dir_pt
    .long write_dir_pt

# The jump table of terminal file descriptor
jump_table_terminal:
    .long open_terminal_pt
    .long read_terminal_pt
    .long close_terminal_pt
    .long write_terminal_pt


.text

# The jump table of the system call
jump_table_syscall:
    # Use a padding.
    .long 0, sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

# The assembly linkage of the system call
# Return -1 if fail, eax if success
system_call:
    # Save all the registers in the correct order.
    pushl %fs
    pushl %es
    pushl %ds
    pushl %gs
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
    # Pop all the registers in the correct order.
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %gs
    popl %ds
    popl %es
    popl %fs
    iret
    # Do not need to modify the %eax value since this is already done in the sub function.
.end
