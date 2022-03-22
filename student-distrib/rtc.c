#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "tests.h"

/* void RTC_init(void);
 * Inputs: void
 * Return Value: none
 * Function: initialize the RTC */
void RTC_init()
{
    uint32_t flags;
    unsigned char prev;
    unsigned char rate = 3;                                          /* The maximux rates is 32784 >> 3 = 4096HZ*/
    cli_and_save(flags);
    outb(REG_B, PORT_0);
    prev = inb(PORT_1);
    outb(REG_B, PORT_0);
    outb(prev | 0x40, PORT_1);                                       /* To initialize the RTC with the sixth bit set.*/
    outb(REG_A, PORT_0);
    prev = inb(PORT_1);
    outb(REG_A, PORT_0);
    outb((prev & 0xF0) | rate, PORT_1);                              /* Mask the value with 0xF0 and set the rates to be 3.*/
    outb(inb(PORT_0) & 0x7F, PORT_0);                                /* Enable NMI*/
    inb(PORT_1);
    outb(REG_C, PORT_0);                                             /* Do some strange stuff with register C.*/
    inb(PORT_1);
    restore_flags(flags);
    sti();
    enable_irq(8);                                                   /* IRQ8 corresponds to rtc.*/
}


/* void RTC_handler(void);
 * Inputs: void
 * Return Value: none
 * Function: handle the RTC */
void RTC_handler()
{
    if (get_counter() == 3)
        test_interrupts();
    send_eoi(8);
    outb(REG_C, PORT_0);                                             /* Do some strange stuff with register C.*/
    inb(PORT_1);
}
