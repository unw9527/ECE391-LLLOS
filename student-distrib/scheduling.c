#include "scheduling.h"
#include "idt.h"
#include "page.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"

int32_t running_term = 0;

/*
 * void pit_init(int hz)
 * Input: none
 * Return Value: none
 * Effect: setup the PIT
 */
void PIT_init()
{
    outb(RES_BYTE, COMMAND_REG);          /* Set our command byte 0x36 */
    outb(DIVISOR & MASK, CHANNEL_0_DP);   /* Set low byte of divisor */
    outb(DIVISOR >> 8, CHANNEL_0_DP);     /* Right shift for one byte */
}

/*
 * void PIT_handler()
 * Input: none
 * Return Value: none
 * Effect: switch to different processes to execute for a slice of time
 */
void PIT_handler(){
    int32_t next_term = (running_term + 1) % MAX_TERMINAL;
    int32_t display_pid = terminal[curr_terminal].prog_array[terminal[curr_terminal].terminal_prog_count - 1]; // curr_pid refers to the terminal that is displayed
    int32_t next_pid;
    if (0 == terminal[next_term].terminal_prog_count){return;} // if no process is running on the next terminal, then do not switch
    next_pid = terminal[next_term].prog_array[terminal[next_term].terminal_prog_count - 1];
    disable_irq(KEYBOARD_IRQ);

    if (next_pid == display_pid){
        enable_irq(KEYBOARD_IRQ);
        // restore_vid_mem();
        // memcpy((void*) terminal[curr_terminal].vid_mem, (const void*)VIDEO, 4096);
    }
    else{
        restore_vid_mem();
        memcpy((void*)VIDEO, (const void*) terminal[next_term].vid_mem, 4096); 
    }

    running_term = next_term;
    send_eoi(0); // 0 is the IRQ number of PIT
}


