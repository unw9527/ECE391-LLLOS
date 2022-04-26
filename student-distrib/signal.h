#ifndef _SIGNAL_H
#define _SIGNAL_H
#include "types.h"
#include "process.h"
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


typedef struct sigpending
{
    struct sigpending* next;                                         /* The pointer to the next signal.*/
    uint32_t signal_index;                                           /* The type of the signal.*/
    uint32_t resources;                                              /* The resources of the signal.*/
} sigpending_t;

typedef struct sigaction
{
    void* sa_handler;                                                /* The function pointer to run as handler.*/
    uint32_t sig_mask;                                               /* The one-mask for the signals to be masked during the hander execution.*/
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

extern sighand_t sighand_array[NUM_PROCESS];

void div_zero_default();
void segfault_default();
void interrupt_default();
void alarm_default();
void user1_default();
void user2_default();
void signal_update(uint32_t type);
void check_pending();

#endif
