#include "scheduling.h"
#include "idt.h"
#include "page.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"
#include "x86_desc.h"
#include "syscall.h"

/* Note:
 * Some code that relates to initialization is from 
 * http://www.osdever.net/bkerndev/Docs/pit.htm
 */

/*
 * void pit_init(int hz)
 * Input: none
 * Return Value: none
 * Effect: setup the PIT
 */
void PIT_init()
{
    running_term = 1;
    curr_terminal = 0;
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
    int32_t run_pid;
    int32_t next_pid;
     // 0 is the IRQ number of PIT
    if (terminal[running_term].terminal_prog_count == 0) {
        schedule = 1;
        asm volatile(
            "movl %%esp, %0       \n"
            "movl %%ebp, %1       \n"
            : "=r" (curr_esp), "=r" (curr_ebp)
            :
            : "memory"
        );
        switch_terminal(running_term);
        send_eoi(0);
        sys_execute((uint8_t*)"shell");
    }
    run_pid = terminal[running_term].prog_array[terminal[running_term].terminal_prog_count-1];
    asm volatile(
        "movl %%ebp, %0       \n"
        "movl %%esp, %1       \n"
        : "=r" (PCB_array[NUM_PROCESS-1-run_pid].thread_info.curr_ebp), "=r" (PCB_array[NUM_PROCESS-1-run_pid].thread_info.curr_esp)
        :
        : "memory"
    );
    running_term = next_term;
    next_pid = terminal[next_term].prog_array[terminal[next_term].terminal_prog_count-1];
    if (terminal[running_term].terminal_prog_count == 0) {
        send_eoi(0);
        return;
    }

    pid = next_pid;
    set_video_page();
    swap_page(next_pid);
    store_vid_mem(running_term);  
    tss.ss0 = KERNEL_DS;
    tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * next_pid - 4;
    send_eoi(0);
    asm volatile(
        "movl %0, %%ebp       \n"
        "movl %1, %%esp       \n"
        :
        : "r" (PCB_array[NUM_PROCESS-1-next_pid].thread_info.curr_ebp), "r" (PCB_array[NUM_PROCESS-1-next_pid].thread_info.curr_esp)
        : "ebp", "esp"
    );
    return;
}


