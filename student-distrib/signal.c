#include "signal.h"
#include "dynamic_alloc.h"
#include "process.h"

sighand_t sighand_array[NUM_PROCESS];

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

void div_zero_default()
{

}

void segfault_default()
{

}

void interrupt_default()
{

}

void alarm_default()
{

}

void user1_default()
{

}

void user2_default()
{

}

void signal_update(uint32_t type)
{
    sigpending_t* pending_element;
    /* Update the pending_exist field.*/
    PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist = 1;
    /* Dynamically allocate a pending element.*/
    pending_element = (sigpending_t*) kmalloc(SIGPENDING_SIZE, __GFP_ZERO && __GFP_NORETRY);
    pending_element->signal_index = type;
    /* Check for resources.*/
    if(type == DIV_ZERO || type == SEGFAULT)
        pending_element->resources = EXCEPTION_RESC;
    else if(type == INTERRUPT)
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

void check_pending()
{
    /* Check whether pending exists.*/
    if (PCB_array[NUM_PROCESS-1-pid].thread_info.pending_exist){
        
    }
    return;
}
