#include "process.h"
#include "filesys.h"
#include "syscall.h"

/* Used to store the several PCB structure */
PCB_t* PCB_array = (PCB_t*) START_PCB;

/* Used to determine the id of the current process */
int32_t pid = -1;

/* Used to stored the available space of the remaining process */
uint8_t process_one_hot[NUM_PROCESS];


/* int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct)
 * Inputs: process_ct -- id of the current process
 *         prev_process_ct -- id of the parent process of the current process
 * Return Value: 0
 * Function: Setup the PCB array based on the process id and the previous process id
 * Side effect: none.
 */
int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct, uint8_t* buf, uint32_t entry_point, int32_t terminal_id)
{
    int i;
    /* Store the current process id and the previous process id.    */
    PCB_array[NUM_PROCESS-1-pid].thread_info.my_index = process_ct;
    PCB_array[NUM_PROCESS-1-pid].thread_info.parent_index = prev_process_ct;
    PCB_array[NUM_PROCESS-1-pid].thread_info.padding = 0;

    PCB_array[NUM_PROCESS-1-pid].thread_info.entry_point = entry_point;

    PCB_array[NUM_PROCESS-1-pid].thread_info.terminal_id = terminal_id;
    /* Store the stdin of the file descriptor array. */
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[0].file_op_pt = (int32_t*)jump_table_terminal;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[0].file_pos = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[0].inode = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[0].flags = 1;

    /* Store the stdout of the file descriptor array. */
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[1].file_op_pt = (int32_t*)jump_table_terminal;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[1].file_pos = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[1].inode = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[1].flags = 1;

    /* Store the arguments.*/
    for (i = 0; i < MAX_BUFFER; i++){
        PCB_array[NUM_PROCESS-1-pid].thread_info.arg_buf[i] = buf[i];
    }

    /* Initialize the signal information.*/
    PCB_array[NUM_PROCESS-1-pid].thread_info.running = 1;
    PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.tail = 0;
    PCB_array[NUM_PROCESS-1-pid].thread_info.sig_dealing = 0;

    /* Store the ebp of its process */
    asm volatile("                         \n\
                movl   (%%ebp), %%eax      \n\
                "
                :"=a"(PCB_array[NUM_PROCESS-1-pid].thread_info.ebp)
    );
    return 0;
}

