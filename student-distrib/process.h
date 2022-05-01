#ifndef _PROCESS_H
#define _PROCESS_H

#include "types.h"
#include "filesys.h"
#include "keyboard.h"
#include "signal.h"
#define KERNEL_STACK 2048
#define NUM_PROCESS 6
#define START_PCB 0x007F4000
#define STACK_BASE 0x00800000
#define USER_SPACE_ESP 0x08300000
#define PROCESS_START 0x08000000
#define PROCESS_END 0x08400000

typedef struct thread_info
{
    uint32_t esp;
    uint32_t ebp;
    uint32_t curr_esp;
    uint32_t curr_ebp;
    int32_t my_index;
    int32_t parent_index;
    file_descriptor_entry_t file_array[DESP_NUM];
    uint8_t arg_buf[MAX_BUFFER];
    int32_t padding;
    int32_t terminal_id;
    uint32_t entry_point;
    int32_t pending_exist;                                           /* This is used to indicate the existence of the pending signal.*/
    sigpending_head_t pending_list;                                  /* A linked list which stores the pending signals.*/
    uint32_t running;
    volatile uint32_t sig_dealing;
} thread_info_t;

typedef struct PCB
{
    union{
        thread_info_t thread_info;
        int32_t stack[KERNEL_STACK];
    };
} PCB_t;

typedef struct task_struct_t
{
    struct task_struct_t* prev;
    struct task_struct_t* next;
    PCB_t* curr_pcb;
} task_struct_t;

int32_t set_up_PCB(int32_t process_ct, int32_t prev_process_ct, uint8_t* buf, uint32_t entry_point, int32_t terminal_id);

extern PCB_t* PCB_array;
extern int32_t pid;
extern uint8_t process_one_hot[NUM_PROCESS];

#endif
