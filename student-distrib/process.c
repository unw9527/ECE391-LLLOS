#include "process.h"
#include "filesys.h"
#include "syscall.h"

PCB_t* PCB_array = (PCB_t*) START_PCB;
int32_t process_counter = -1;
uint8_t process_one_hot[NUM_PROCESS];

int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct)
{
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.my_index = process_ct;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.parent_index = prev_process_ct;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[0].file_op_pt = (int32_t*)jump_table_terminal;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[0].file_pos = 0;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[0].inode = 0;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[0].flags = 1;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[1].file_op_pt = (int32_t*)jump_table_terminal;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[1].file_pos = 0;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[1].inode = 0;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.file_array[1].flags = 1;

    asm volatile("            \n\
        movl   %%ebp, %%eax   \n\
        "
        :"=a"(PCB_array[NUM_PROCESS-1-process_counter].thread_info.ebp) );
    return 0;
}
