#include "scheduling.h"
#include "idt.h"
#include "page.h"
#include "process.h"
#include "lib.h"
#include "terminal.h"

/*
 * void pit_init(int hz)
 * Input: frequency
 * Return Value: none
 * Effect: setup the PIT
 */
void pit_init(int hz)
{
    int divisor = 1193180 / hz;   /* Calculate our divisor */
    outb(0x36, 0x43);             /* Set our command byte 0x36 */
    outb(divisor & 0xFF, 0x40);   /* Set low byte of divisor */
    outb(divisor >> 8, 0x40);     /* Set high byte of divisor */
}

/*
 * void switch_process()
 * Input: none
 * Return Value: none
 * Effect: switch to different processes to execute for a slice of time
 */
void switch_process(){
    int next_term = (curr_terminal + 1) % MAX_TERMINAL;

}


