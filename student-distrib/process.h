#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include "filesys.h"
#define KERNEL_STACK 2048
#define NUM_PROCESS 6
#define START_PCB 0x007F4000
#define STACK_BASE 0x00800000
#define USER_SPACE_ESP 0x08300000

typedef struct thread_info
{
    uint32_t esp;
    uint32_t ebp;
    int32_t my_index;
    int32_t parent_index;
    file_descriptor_entry_t file_array[DESP_NUM];
} thread_info_t;

typedef struct PCB
{
    union{
        thread_info_t thread_info;
        int32_t stack[KERNEL_STACK];
    };
} PCB_t;

int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct);

extern PCB_t* PCB_array;
extern int32_t process_counter;
extern uint8_t process_one_hot[NUM_PROCESS];

#endif
