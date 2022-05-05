#include "syscall.h"
#include "filesys.h"
#include "keyboard.h"
#include "terminal.h"
#include "process.h"
#include "page.h"
#include "idt.h"
#include "x86_desc.h"
#include "scheduling.h"
#include "dynamic_alloc.h"

/* int32_t sys_halt (uint8_t status)
 * Inputs: status -- the status of the halt process
 * Return Value: 0
 * Function: Halt the current process. If the current 
 *           process is "shell", reboot the "shell"
 * Side effect: none.
 * 
 */
int32_t sys_halt (uint8_t status) {
    int i;                                      /* Variable of the "for" loop.  */
    uint32_t ebp_val;                           /* Store the ebp value of the parent process.  */
    uint32_t esp_val;
    uint32_t extend_status;
    cli();

    terminal[running_term].terminal_prog_count--;
    process_one_hot[pid] = 0;       /* Clear the assosiate process_one_hot entry.  */
    /* Close the all the file descriptor (except for stdin and stdout).  */
    for (i = 2; i < DESP_NUM; i++) {
        if (PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[i].flags)
            sys_close(i);
        /* Set the flags to 0.  */
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[i].flags = 0;
    }

    /* Determine whether the current processor is shell     */
    if (terminal[running_term].terminal_prog_count == 0) {
        sys_execute((uint8_t *) "shell");       // If the shell is close, reboot it
    }

    /* Restore parent paging */
    swap_page(terminal[running_term].prog_array[terminal[running_term].terminal_prog_count - 1]);

    /* Store the previous ebp value.    */
    ebp_val = PCB_array[NUM_PROCESS-1-pid].thread_info.ebp;
    esp_val = PCB_array[NUM_PROCESS-1-pid].thread_info.esp;

    /* Update the pid.      */
    pid = terminal[running_term].prog_array[terminal[running_term].terminal_prog_count - 1];

    /* Restore the kernel stack pointer.    */
    tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * pid - 4;     // Make more space

    /* Check exception.*/
    if (exception_happen == 1){
        exception_happen = 0;
        extend_status = EXCEPTION_ERROR;
    }

    else{
        extend_status = (uint32_t)status;
    }
    sti();
    /* Restore the previous ebp.  */
    asm volatile (
                 "mov %0, %%eax;"
                 "mov %1, %%ebp;"
                 "mov %2, %%esp;"
                 "jmp RET_FROM_PROCESS"
                 :
                 :"r"(extend_status), "r"(ebp_val), "r"(esp_val)
                 :"%eax"
 	);
    return 0;
}

/* int32_t sys_execute (const uint8_t* command)
 * Inputs: command -- space-separated command of execute
 * Return Value: status
 * Function: Execute the given process
 * Side effect: none.
 * 
 */
int32_t sys_execute (const uint8_t* command) {
    cli();
    int i, j;                           // Variable of the "for" loop. 
    uint8_t buf1[MAX_BUFFER];           // Store the file name
    uint8_t buf2[EXECUTE_LEN+1];        // Store the file data
    uint8_t buf3[MAX_BUFFER];           // Store arguments.
    /* Store the file information   */
    dentry_t file_dentry;
    uint32_t inode_num;
    uint32_t entry_point = 0;
    int32_t prev_pid;
    /* ----------------- Step1. Obtain the file name and the arguments ------------------*/
    /* buf1 contains the file name.*/
    for (i = 0; i < MAX_BUFFER; i++){
        if (command[i] == SPACE || command[i] == 0)
            break;
        buf1[i] = command[i];
    }
    if (i < MAX_BUFFER)
        buf1[i] = 0;
    /* This sequence of code is to store the argument into buf3.*/
    if (command[i] == SPACE){
        for (j = 0; j < MAX_BUFFER; j++){
            if (command[i+j+1] == 0)
                break;
            buf3[j] = command[i+j+1];
        }
        if (j < MAX_BUFFER)
            buf3[j] = 0;
    }
    /* Check whether terminated with NULL.*/
    else{
        if (command[i] == 0)
            buf3[0] = 0;
    }

    /* --------------------------- Step2. Check executable ----------------------------*/

    if (read_dentry_by_name(buf1, &file_dentry) == -1)
        return -1;
    inode_num = (uint32_t)file_dentry.inode_num;
    for (i = 0; i <= 3; i++){
        buf2[i] = 0;
    }

    /* Now buf2 is used as the buffer to hold the read data.*/
    read_data(inode_num, 0, buf2, EXECUTE_LEN);

    /* If this is not an executable, return -1.*/
    if (buf2[0] != MAGIC1 || buf2[1] != MAGIC2 || buf2[2] != MAGIC3 || buf2[3] != MAGIC4)
        return -1;

    /* Get the entrypoint.  */
    for (i = 0; i <= 3; i++) {
        entry_point = entry_point << 8;
        entry_point |= buf2[HIGH_ADDR - i];
    }
    /* --------------------------- Step3. Swap the page ----------------------------*/
    /* Check the free process index.*/
    for (i = 0; i < NUM_PROCESS; i++){
        if (process_one_hot[i] == 0){
            process_one_hot[i] = 1;
            prev_pid = pid;
            pid = i;
            break;
        }
    }

    /* If no free process index, return -1.*/
    if (i == NUM_PROCESS)
        return -1;
    
    terminal[running_term].prog_array[terminal[running_term].terminal_prog_count] = pid;
    terminal[running_term].terminal_prog_count += 1;

    swap_page(pid);

    /* --------------------------- Step4. Load the executable into 0x08048000 ----------------------------*/
    file_loader(&file_dentry);

    /* --------------------------- Step5. Set up the PCB ----------------------------*/
    set_up_PCB(pid, prev_pid, buf3, entry_point, running_term);
    /* I do not think we need this.*/
    if (schedule) {
        PCB_array[NUM_PROCESS-1-pid].thread_info.curr_esp = curr_esp;
        PCB_array[NUM_PROCESS-1-pid].thread_info.curr_ebp = curr_ebp;
        schedule = 0;
    }
    else{
        PCB_array[NUM_PROCESS-1-pid].thread_info.curr_esp = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.curr_ebp = 0;
    }

    asm volatile("                         \n\
                movl   %%esp, %%eax      \n\
                "
                :"=a"(PCB_array[NUM_PROCESS-1-pid].thread_info.esp)
    );
    /* --------------------------- Step6. Create the process and switch ----------------------------*/
    /* Set the TSS's ss0 value.*/
    tss.ss0 = KERNEL_DS;

    /* Set the esp0 value.*/
    tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * pid - 4;

    /* Modify the stack.    */
    asm volatile("                                                \n\
        pushw $0                                                  \n\
        pushw %0                                                  \n\
        pushl %1                                                  \n\
        pushfl                                                    \n\
        popl %%ecx                                                \n\
        orl $0x200,%%ecx                                          \n\
		pushl %%ecx                                               \n\
        pushw $0                                                  \n\
        pushw %2                                                  \n\
        pushl %3                                                  \n\
        movw %0, %%cx                                             \n\
        movw %%cx, %%ds                                           \n\
        iret                                                      \n\
        RET_FROM_PROCESS:                                         \n\
        leave                                                     \n\
        ret                                                       \n\
        "                                                           \
    : /* no output*/                                                \
    : "g"((USER_DS)), "g"((USER_SPACE_ESP)), "g"((USER_CS)), "g"((entry_point))\
    : "memory", "%ecx"/* no register modification*/                 \
    );
    return 0; 
}


/* int32_t open(const uint8_t* filename);
 * Inputs: filename: The name of the file.
 * Return Value: -1 if not success. 0 if success.
 * Function: Open the file and fill in the information about the file for further operation. */
/* Side effect: none.*/
int32_t sys_open (const uint8_t* filename)
{
    int32_t fd;
    int32_t (**pt)(const uint8_t*);
    dentry_t dentry;

    /* If the file does not exists, return -1.*/
    if (read_dentry_by_name(filename, &dentry) == -1)
        return -1;
    for (fd = 2; fd <= 7; fd++){
        if (PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags == 0){
            break;
        }
    }
    /* If there is no free fd, return -1.*/
    if (fd == 8)
        return -1;
    switch (dentry.filetype)
    {
    /* The rtc.*/
    case 0:
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_rtc;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].inode = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags = 1;
        break;
    /* The directory.*/
    case 1:
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_dir;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].inode = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags = 1;
        break;
    /* The regular file.*/
    case 2:
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_file;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].inode = dentry.inode_num;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags = 1;
        break;
    default:
        break;
    }
    /* Call the specific open operation.*/
    pt = (int32_t (**)(const uint8_t*)) PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt[0];
    (**pt)(filename);
    return fd;
}

/* int32_t close(int32_t fd);
 * Inputs: fd: The file descriptor.
 * Return Value: -1 if fd==0 or fd==1. 0 if success.
 * Function: Close the file. */
/* Side effect: Close the file.*/
int32_t sys_close (int32_t fd)
{
    int32_t (**pt)(int32_t);

    /* If fd corresponds to stdin and stdout, return -1.*/
    if (fd == 0 || fd == 1 || fd < 0 || fd > 7)
        return -1;
    pt = (int32_t (**)(int32_t)) PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt[2];
    /* Call the specific close function.*/
    (**pt)(fd);
    if (PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags == 0)
        return -1;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags = 0;
    return 0;
}

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to write. nbytes: #bytes to write.
 * Return Value: Number of bytes read. -1 indicates failure.
 * Function: Read the file. */
/* Side effect: none.*/
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes)
{
    int32_t bytes_read;
    int32_t (**pt)(int32_t, void* , int32_t);

    if (fd < 0 || fd > 7 || fd == 1)
        return -1;
    if (PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags == 0)
        return -1;
    pt = (int32_t (**)(int32_t, void* , int32_t)) PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt[1];
    /* Call the specific read function.*/
    bytes_read = (**pt) (fd, buf, nbytes);
    return bytes_read;
}

/* int32_t write (int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to write. nbytes: #bytes to write.
 * Return Value: 0.
 * Function: Write the file. */
/* Side effect: none.*/
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes)
{
    int32_t bytes_written;
    int32_t (**pt)(int32_t, const void*, int32_t);
    if (fd <= 0 || fd > 7)
        return -1;

    if (PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].flags == 0)
        return -1;
    // printf("rt = %d\n", running_term);
    pt = (int32_t (**)(int32_t, const void*, int32_t)) PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_op_pt[3];
    bytes_written = (**pt) (fd, buf, nbytes);
    return bytes_written;
}

/* int32_t sys_getargs (uint8_t* buf, int32_t nbytes)
 * Inputs: buf: The buffer to write. nbytes: #bytes to write.
 * Return Value: 0 if success, -1 otherwise.
 * Function: Copy the argument from kernel to user. */
/* Side effect: none.*/
int32_t sys_getargs (uint8_t* buf, int32_t nbytes)
{
    uint32_t length;

    length = strlen((const int8_t*)PCB_array[NUM_PROCESS-1-pid].thread_info.arg_buf);
    /* If the argument does not fit into buffer or null pointer or null string, return -1.*/
    if (buf == 0 || length == 128 || PCB_array[NUM_PROCESS-1-pid].thread_info.arg_buf[0] == 0)
        return -1;
    strncpy((int8_t*)buf, (int8_t*)PCB_array[NUM_PROCESS-1-pid].thread_info.arg_buf, nbytes);
    return 0;
}

/* int32_t sys_getargs sys_vidmap(uint8_t** screen_start)
 * Inputs: screen_start: The address of the memory pointer.
 * Return Value: 0 if success, -1 otherwise.
 * Function: Map a virtual address to vedio memory and return the virtual address.*/
/* Side effect: Change the pages.*/
int32_t sys_vidmap(uint8_t** screen_start)
{
    /* Check for Null pointer.*/
    if ((uint32_t)screen_start < PROCESS_START || (uint32_t)screen_start >= PROCESS_END)
        return -1;
    /* Create a page table for the user-level video memory.*/
    set_video_page(running_term);
    /* Set the screen start.*/
    *screen_start = (uint8_t*) USER_VIDEO_ADDR;
    return USER_VIDEO_ADDR;
}

/* int32_t sys_set_handler (int32_t signum, void* handler) */
/* Side effect: Need to implement for extra credit.*/
int32_t sys_set_handler (int32_t signum, void* handler)
{
    /* Check the range of the handler.*/
    if ((uint32_t)handler < PROCESS_START || (uint32_t)handler >= PROCESS_END)
        return -1;
    /* Set the handler.*/
    sighand_array[pid].action[signum].sa_handler = handler;
    return 0;
}

/* int32_t sys_sigreturn (void) */
/* Side effect: Need to implement for extra credit.*/
int32_t sys_sigreturn (void)
{
    int32_t* esp;
    int32_t* ebp;
    int32_t cs;
    /* Free the pointer.*/
    cli();
    kfree(sig_pt);
    PCB_array[NUM_PROCESS-1-pid].thread_info.sig_dealing = 0;
    asm volatile("                                                \n\
        movl 68(%%ebp), %0                                        \n\
        movl 52(%0), %1                                           \n\
        movl %%ebp, %2                                            \n\
        "                                                           \
    : "=r" (esp), "=r" (cs), "=r" (ebp)                             \
    : /* no input*/                                                 \
    : "memory"                                                      \
    );
    /* Check for kernel or user.*/
    if (cs == KERNEL_CS){
        memcpy(ebp+4, esp, HARDWARE_CONTEXT-8);
        return SIGCASE1;
    }   
    else{
        memcpy(ebp+2, esp, HARDWARE_CONTEXT);
        return SIGCASE2;
    }
    /* Note that it is not allowed in the general case though.*/
}

void* sys_malloc(int32_t size)
{
    void* pt;
    pt = kmalloc(size, __GFP_NORETRY | __GFP_ZERO);
    return pt;
}

int32_t sys_free(void* addr)
{
    kfree(addr);
    return 0;
}
