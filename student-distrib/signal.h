#ifndef _SIGNAL_H
#define _SIGNAL_H

#include "types.h"
#define QUEUE_SIZE 10
#define MAX_SIGNAL 32
#define EXCEPTION_RESC 0
#define INTERRUPT_RESC 1
#define SYSCALL_RESC 2
#define DIV_ZERO 0
#define SEGFAULT 1
#define INTERRUPT 2
#define ALARM 3
#define USER1 4
#define USER2 5
#define ALL_MASK 0x3f
#define SIGPENDING_SIZE 12
#define SIGNAL_SIZE 512
#define EBP_OFFSET 3
#define HARDWARE_CONTEXT 68
#define MIDDLE_POS 64
#define SIGCASE1 0x888
#define SIGCASE2 0x999


typedef struct sigpending
{
    struct sigpending* next;                                         /* The pointer to the next signal.*/
    uint32_t signal_index;                                           /* The type of the signal.*/
    uint32_t resources;                                              /* The resources of the signal.*/
} sigpending_t;

typedef struct sigaction
{
    void* sa_handler;                                                
    uint32_t sig_mask;                                              
} sigaction_t;

typedef struct sighand
{
    uint32_t count;                                                  /* Currently # signals used.*/
    sigaction_t action[MAX_SIGNAL];                                  /* 32 maximum sigaction structure for different signals.*/
} sighand_t;

typedef struct sigpending_head
{
    sigpending_t* head;
    sigpending_t* tail;
} sigpending_head_t;

extern sighand_t sighand_array[6];
extern void* sig_pt;

int32_t sighand_init();
void div_zero_default();
void segfault_default();
void interrupt_default();
void alarm_default();
void user1_default();
void kernel_dispatcher(uint32_t type);
void user2_default();
void signal_update(uint32_t type);
int32_t check_pending(uint32_t cs);
void user_signal(uint32_t signal_index);

#endif
