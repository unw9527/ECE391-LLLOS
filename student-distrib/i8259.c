/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    cli();

    master_mask = 0xff;
    slave_mask = 0xff;

    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);
    outb(ICW4, MASTER_8259_PORT + 1);

    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);
    outb(ICW4, SLAVE_8259_PORT + 1);

    outb(0xff, MASTER_8259_PORT + 1);
    outb(0xff, SLAVE_8259_PORT + 1);

    sti();
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    cli();

    if (irq_num >= 8) {
        irq_num = irq_num - 8;
        slave_mask &= (~(0x1 << irq_num));
        outb(slave_mask, SLAVE_8259_PORT + 1);
        master_mask &= 0xFB;
        outb(master_mask, MASTER_8259_PORT + 1);
    }
    else {
        master_mask &= (~(0x1 << irq_num));
        outb(master_mask, MASTER_8259_PORT + 1);
    }
    sti();
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    cli();
    if (irq_num >= 8) {
        irq_num = irq_num - 8;
        slave_mask |= (0x1 << irq_num);
        outb(slave_mask, SLAVE_8259_PORT + 1);
    }
    else {
        master_mask |= (0x1 << irq_num);
        outb(master_mask, MASTER_8259_PORT + 1);
    }
    sti();

}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    cli();
    if (irq_num >= 8) {
        outb(EOI + (irq_num - 8), SLAVE_8259_PORT);
        outb(EOI + 0x2, MASTER_8259_PORT);
    }
    else 
        outb(EOI + irq_num, SLAVE_8259_PORT);
    sti();
}

