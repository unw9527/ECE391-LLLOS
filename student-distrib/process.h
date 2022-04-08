#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include "filesys.h"
#define KERNEL_STACK 2048
#define NUM_PROCESS 6
#define START_PCB 0x007F4000

typedef struct thread_info
{
    int32_t my_index;
    int32_t parent_index;
    int32_t return_address;
    file_descriptor_entry_t file_array[DESP_NUM];
} thread_info_t;

typedef struct PCB
{
    union{
        thread_info_t thread_info;
        int32_t stack[KERNEL_STACK];
    };
} PCB_t;

extern PCB_t* PCB_array;

#endif
