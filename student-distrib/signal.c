// Please refer to fsdir/reference.txt for the reference

#include "signal.h"
#include "dynamic_alloc.h"
#include "process.h"
#include "page.h"
#include "syscall.h"
#include "terminal.h"
#include "scheduling.h"
#include "x86_desc.h"

sighand_t sighand_array[NUM_PROCESS];
extern int32_t SIGNAL_LINKAGE[5];
void* sig_pt;


/**
 * @brief handling the signal
 * 
 * @return 0
 */
int32_t sighand_init()
{
    int32_t i;
    for (i = 0; i < NUM_PROCESS; i++){
        sighand_array[i].count = 6;
        sighand_array[i].action[0].sa_handler = div_zero_default;
        sighand_array[i].action[1].sa_handler = segfault_default;
        sighand_array[i].action[2].sa_handler = interrupt_default;
        sighand_array[i].action[3].sa_handler = alarm_default;
        sighand_array[i].action[4].sa_handler = user1_default;
        sighand_array[i].action[5].sa_handler = user2_default;
        sighand_array[i].action[0].sig_mask = ALL_MASK;
        sighand_array[i].action[1].sig_mask = ALL_MASK;
        sighand_array[i].action[2].sig_mask = ALL_MASK;
        sighand_array[i].action[3].sig_mask = ALL_MASK;
        sighand_array[i].action[4].sig_mask = ALL_MASK;
        sighand_array[i].action[5].sig_mask = ALL_MASK;
    }
    return 0;
}


/**
 * @brief the dispatcher
 * 
 * @param type error type
 */
void kernel_dispatcher(uint32_t type)
{
    switch(type){
        case 0:
            div_zero_default();
            break;
        case 1:
            segfault_default();
            break;
        case 2:
            interrupt_default();
            break;
        case 3:
            alarm_default();
            break;
        case 4:
            user1_default();
            break;
        case 5:
            user2_default();
            break;
        default:
            break;
    }
    /* This return sometimes never reached.*/
    return;
}

/**
 * @brief division by 0
 * 
 */
void div_zero_default()
{
    /* Kill the program.*/
    sys_halt(0);
}

/**
 * @brief segfault
 * 
 */
void segfault_default()
{
    /* Kill the program.*/
    sys_halt(0);
}

/**
 * @brief interrupt
 * 
 */
void interrupt_default()
{
    /* Kill the program.*/
    signal_flag[curr_terminal] = 1;
}

/**
 * @brief alarm
 * 
 */
void alarm_default()
{
    /* Ignored.*/
    //printf("ALARM for 10s.\n");
    return;
}

/**
 * @brief user1
 * 
 */
void user1_default()
{
    /* Sleep the program.*/
    if (sleep_num <= 1 && sleep_flag[curr_terminal] == 0){
        sleep_num++;
        sleep_flag[curr_terminal] = 1;
    }
    return;
}

/**
 * @brief user2
 * 
 */
void user2_default()
{
    /* Wake up the program.*/
    if (sleep_flag[curr_terminal]){
        sleep_num--;
        sleep_flag[curr_terminal] = 0;
    }
    return;
}

/**
 * @brief update the signal according to type
 * 
 * @param type 
 */
void signal_update(uint32_t type)
{
    sigpending_t* pending_element;
    /* Update the pending_exist field.*/
    if (pid == -1)
        return;
    PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist = 1;
    /* Dynamically allocate a pending element.*/
    pending_element = (sigpending_t*) kmalloc(SIGPENDING_SIZE, __GFP_ZERO | __GFP_NORETRY);
    pending_element->signal_index = type;
    /* Check for resources.*/
    if(type == DIV_ZERO || type == SEGFAULT)
        pending_element->resources = EXCEPTION_RESC;
    else if(type == INTERRUPT || type == USER1 || type == USER2)
        pending_element->resources = INTERRUPT_RESC;
    else if (type == ALARM)
        pending_element->resources = SYSCALL_RESC;
    pending_element->next = 0;
    /* Check whether the list is empty.*/
    if (!PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head){
        PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head = pending_element;
        PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.tail = pending_element;
    }
    /* The case thaht the pending list is not empty.*/
    else{
        PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.tail->next = pending_element;
        PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.tail = pending_element;
    }
    return;
}

/**
 * @brief check whether the state is pending
 * 
 * @param cs code segment
 * @return signal_index
 */
int32_t check_pending(uint32_t cs)
{
    sigpending_t* pending_element;
    uint32_t signal_index;
    /* Check whether pending exists.*/
    if (pid == -1)
        return -1;
    pending_element = PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head;
    if (PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist && PCB_array[NUM_PROCESS-1-pid].thread_info.sig_dealing == 0){
        /* Remove the pending element from the list.*/
        /* Case1, pending element is the last element in the list.*/
        if (!pending_element->next){
            PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist = 0;
            PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head = 0;
            PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.tail = 0;
        }
        /* Case2, pending element is not the tail of the list.*/
        else
            PCB_array[NUM_PROCESS-1-pid].thread_info.pending_list.head = pending_element->next;
        /* Execute the function stored in the function pointer.*/
        signal_index = pending_element->signal_index;
        /* Free the element.*/
        kfree(pending_element);
        if ((uint32_t)sighand_array[pid].action[signal_index].sa_handler < KERNEL_END)
            kernel_dispatcher(signal_index);
        else if (cs != KERNEL_CS){
            PCB_array[NUM_PROCESS-1-pid].thread_info.sig_dealing = 1;
            return (int32_t)signal_index;
        }
    }
    return -1;
}

/**
 * @brief User signal
 * 
 * @param signal_index the index of the signal
 */
void user_signal(uint32_t signal_index)
{
    int32_t* ebp;
    /* Allocate a dynamic space for the signal context.*/
    sig_pt = kmalloc(SIGNAL_SIZE, __GFP_NORETRY | __GFP_ZERO);
    ((int32_t**)sig_pt)[MIDDLE_POS] = (int32_t*)sig_pt + HARDWARE_CONTEXT / 4 + 2 + MIDDLE_POS;
    ((uint32_t*)sig_pt)[MIDDLE_POS + 1] = signal_index;
    asm volatile(
        "movl %%ebp, %0       \n"
        : "=r" (ebp)
        : /* None*/
        : "memory", "cc"
    );
    /* Copy the right information into the dynamic memory.*/
    memcpy((int32_t*)sig_pt + 2 + MIDDLE_POS, ebp + EBP_OFFSET, HARDWARE_CONTEXT);
    memcpy((int32_t*)sig_pt + 2 + HARDWARE_CONTEXT / 4 + MIDDLE_POS, SIGNAL_LINKAGE, 3 + HARDWARE_CONTEXT / 4);
    /* Exploit iret to go to user level.*/
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
        "                                                           \
    : /* no output*/                                                \
    : "g"(USER_DS), "g"((int32_t*)sig_pt + MIDDLE_POS), "g"(USER_CS), "g"(sighand_array[pid].action[signal_index].sa_handler)\
    : "memory", "%ecx", "cc"                                        \
    );
    return;
}
