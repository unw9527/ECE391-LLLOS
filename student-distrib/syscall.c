#include "syscall.h"
#include "filesys.h"
#include "keyboard.h"
#include "terminal.h"
#include "process.h"
#include "page.h"
#include "x86_desc.h"

/* int32_t open(const uint8_t* filename);
 * Inputs: filename: The name of the file.
 * Return Value: -1 if not success. 0 if success.
 * Function: Open the file and fill in the information about the file for further operation. */
/* Side effect: none.*/

int32_t sys_halt (uint8_t status)
{
    int i;

    process_one_hot[process_counter] = 0;
    
    for (i = 2; i < DESP_NUM; i++) {
        if (PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[i].flags)
            sys_close(i);
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[i].flags = 0;
    }

    if (PCB_array[NUM_PROCESS-1-process_counter].thread_info.parent_index == -1) {
        process_counter = -1;
        sys_execute((uint8_t *) "shell");
    }
    /* Restore parent paging */
    swap_page(PCB_array[NUM_PROCESS-1-process_counter].thread_info.parent_index);
    process_counter = PCB_array[NUM_PROCESS-1-process_counter].thread_info.parent_index;
    // tss.esp0 = current pcb's esp
    tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * process_counter - 4;
    // !!! ebp of PCB
    asm volatile (
                 "mov %0, %%eax;"
                 "mov %1, %%ebp;"
                 "jmp RET_FROM_PROCESS;"
                 :
                 :"r"((uint32_t) status), "r"(PCB_array[NUM_PROCESS-1-process_counter].thread_info.ebp)
                 :"%eax"
 	);
    return 0;
}

int32_t sys_execute (const uint8_t* command)
{
    int i;
    int j;
    uint8_t buf1[MAX_BUFFER];
    uint8_t buf2[MAX_BUFFER];
    dentry_t file_dentry;
    uint32_t inode_num;
    uint32_t entry_point = 0;
    int32_t prev_process_counter;
    uint16_t ss_val;
    uint32_t esp0_val;
    uint32_t user_space_esp;
    uint16_t user_space_ss;
    uint16_t user_space_cs;
    uint32_t user_space_eip;
    int32_t return_val;
    /* Step1. Obtain the file name and the arguments.*/
    /* buf1 contains the file name.*/
    for (i = 0; i < MAX_BUFFER; i++){
        if (command[i] == SPACE || command[i] == 0)
            break;
        buf1[i] = command[i];
    }
    buf1[i] = 0;
    /* This sequence of code is to store the argument into buf2*/
    if (command[i] == SPACE){
        for (j = 0; j < MAX_BUFFER; j++){
            if (command[i+j+1] == 0)
                break;
            buf2[j] = command[i+j+1];
        }
        buf2[j] = 0;
    }
    /* Step2. Check executable.*/
    if (read_dentry_by_name(buf1, &file_dentry) == -1)
        return -1;
    inode_num = (uint32_t)file_dentry.inode_num;
    /* Now buf2 is used as the buffer to hold the read data.*/
    read_data(inode_num, 0, buf2, EXECUTE_LEN);
    /* If this is not an executable, return -1.*/
    if (buf2[0] != MAGIC1 || buf2[1] != MAGIC2 || buf2[2] != MAGIC3 || buf2[3] != MAGIC4)
        return -1;
    /* Get the entrypoint.*/
    for (i = 0; i <= 3; i++){
        entry_point = entry_point << 8;
        entry_point |= buf2[HIGH_ADDR-i];
    }
    /* Step3. Swap the page.*/
    /* Check the free process index.*/
    for (i = 0; i <= 5; i++){
        if (process_one_hot[i] == 0){
            process_one_hot[i] = 1;
            prev_process_counter = process_counter;
            process_counter = i;
            break;
        }
    }
    /* If no free process index, return -1.*/
    if (i == 6)
        return -1;
    swap_page(process_counter);
    /* Step4. Load the executable into 0x08048000*/
    file_loader(&file_dentry);
    /* Step5. Set up the PCB.*/
    set_up_PCB(process_counter, prev_process_counter);
    /* Step6. Create the process and switch.*/
    /* Set the TSS's ss0 value.*/
    asm volatile ("                                               \n\
        movw %%ss, %0											  \n\
    	"                                                           \
	: /* no outputs */                                          	\
	: "g"((ss_val))                                                 \
	: "memory", "cc"                                                \
 	);
    tss.ss0 = ss_val;
    /* Set the esp0 value.*/
    esp0_val = STACK_BASE - 4 * KERNEL_STACK * process_counter - 4;
    tss.esp0 = esp0_val;
    /* Get the value needed to push onto the stack.*/
    user_space_esp = USER_SPACE_ESP;
    /* espare already set.*/
    user_space_eip = entry_point;
    user_space_cs = USER_CS;
    user_space_ss = USER_DS;
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
    : "g"((user_space_ss)), "g"((user_space_esp)), "g"((user_space_cs)), "g"((user_space_eip)) \
    : "memory", "%ecx"/* no register modification*/                 \
    );
    return 0; 

}


int32_t sys_open (const uint8_t* filename)
{
    int32_t fd;
    int32_t (**pt)(const uint8_t*);
    dentry_t dentry;
    /* If the file does not exists, return -1.*/
    if (read_dentry_by_name(filename, &dentry) == -1)
        return -1;
    for (fd = 2; fd <= 7; fd++){
        if (PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags == 0){
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
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_rtc;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].inode = 0;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags = 1;
        break;
    /* The directory.*/
    case 1:
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_dir;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].inode = 0;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags = 1;
        break;
    /* The regular file.*/
    case 2:
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt = (int32_t*)jump_table_file;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].inode = dentry.inode_num;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_pos = 0;
        PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags = 1;
        break;
    default:
        break;
    }
    /* Call the specific open operation.*/
    pt = (int32_t (**)(const uint8_t*)) PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt[0];
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
    if (fd == 0 || fd == 1)
        return -1;
    pt = (int32_t (**)(int32_t)) PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt[2];
    /* Call the specific close function.*/
    (**pt)(fd);
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags = 0;
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
    if (PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags == 0)
        return -1;
    pt = (int32_t (**)(int32_t, void* , int32_t)) PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt[1];
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
    if (PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].flags == 0)
        return -1;
    pt = (int32_t (**)(int32_t, const void*, int32_t)) PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[fd].file_op_pt[3];
    bytes_written = (**pt) (fd, buf, nbytes);
    return bytes_written;
}

int32_t sys_getargs (uint8_t* buf, int32_t nbytes)
{
    return -1;
}
