#include "process.h"
#include "filesys.h"

PCB_t* PCB_array = (PCB_t*) START_PCB;
int32_t process_counter = -1;
uint8_t process_one_hot[NUM_PROCESS];

int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct)
{
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.my_index = process_ct;
    PCB_array[NUM_PROCESS-1-process_counter].thread_info.parent_index = prev_process_ct;
    return 0;
}
