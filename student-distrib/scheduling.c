#include "scheduling.h"
#include "idt.h"
#include "page.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"
#include "x86_desc.h"
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
    send_eoi(0); // 0 is the IRQ number of PIT
    next_pid = terminal[next_term].prog_array[terminal[next_term].terminal_prog_count - 1];
    running_term = next_term;
    if (0 == terminal[next_term].terminal_prog_count || 1 == terminal[next_term].terminal_prog_count || curr_terminal == next_term){return;} // if no process is running on the next terminal, then do not switch
    swap_page(next_pid);
    entry_point = PCB_array[NUM_PROCESS-1-next_pid].thread_info.entry_point;
    asm volatile ("                                               \n\
        movw %%ss, %0											  \n\
    	"                                                           \
	: /* no outputs */                                          	\
	: "g"((ss_val))                                                 \
	: "memory", "cc"                                                \
 	);
    tss.ss0 = ss_val;

    /* Set the esp0 value.*/
    tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * next_pid - 4; ;

    /* Modify the stack.    */
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
        pushw %3                                                  \n\
        movw %0, %%cx                                             \n\
        movw %%cx, %%ds                                           \n\
        iret                                                      \n\
        "                                                           \
    : /* no output*/                                                \
    : "g"((USER_DS)), "g"((USER_SPACE_ESP)), "g"((USER_CS)), "g"((entry_point))\
    : "memory", "%ecx"/* no register modification*/                 \
    );

}


