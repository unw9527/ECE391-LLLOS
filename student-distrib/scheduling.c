#include "scheduling.h"
#include "idt.h"
#include "page.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"
#include "x86_desc.h"

/*
 * void pit_init(int hz)
 * Input: none
 * Return Value: none
 * Effect: setup the PIT
 */
void PIT_init()
{
    running_term = 0;
    outb(RES_BYTE, COMMAND_REG);          /* Set our command byte 0x36 */
    outb(DIVISOR & MASK, CHANNEL_0_DP);   /* Set low byte of divisor */
    outb(DIVISOR >> 8, CHANNEL_0_DP);     /* Right shift for one byte */
    enable_irq(0);
}

/*
 * void PIT_handler()
 * Input: none
 * Return Value: none
 * Effect: switch to different processes to execute for a slice of time
 */
void PIT_handler(){
    int32_t next_term = (running_term + 1) % MAX_TERMINAL;
    // int32_t display_pid = terminal[curr_terminal].prog_array[terminal[curr_terminal].terminal_prog_count - 1]; // curr_pid refers to the terminal that is displayed
    int32_t next_pid;
    uint32_t ss_val;
    uint32_t entry_point;
     // 0 is the IRQ number of PIT
    if (terminal[next_term].terminal_prog_count == 0) {
        send_eoi(0);
        running_term = next_term;
        sys_execute((uint8_t*)"shell");
        return;
    }
    send_eoi(0);
    running_term = next_term;
    
    // set_video_page();

    // store_vid_mem(running_term);  
    // // swap_page(next_pid);
    
    // tss.ss0 = KERNEL_DS;
    // tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * next_pid - 4;

    // asm volatile(
    //     "movl %0, %%ebp       \n"
    //     :
    //     : "r" (PCB_array[NUM_PROCESS-1-next_pid].thread_info.ebp)
    //     : "ebp"
    // );
}


