.globl en_pg

# enable the page
# Input: None
# Return Value: 0
# Function: Enable paging
en_pg:
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    movl 8(%ebp), %ebx
    movl %ebx, %cr3
    movl %cr4, %ebx
    orl $0x00000010, %ebx
    movl %ebx, %cr4
    movl %cr0, %ebx
    orl $0x80000000, %ebx
    movl %ebx, %cr0
    movl $0, %eax
    popl %edi
    popl %esi
    popl %ebx
    leave
    ret
.end

