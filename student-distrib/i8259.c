/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/* void i8259_init(void);
 * Inputs: void
 * Return Value: none
 * Function: initialize i8259 */
/* Initialize the 8259 PIC */
void i8259_init(void) {
    // Mask all the IRQ of master PIC and slave PIC
    master_mask = MASK_ALL;
    slave_mask = MASK_ALL;

    // Initial the master PIC
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT + ICW4);
    outb(ICW3_MASTER, MASTER_8259_PORT + ICW4);
    outb(ICW4, MASTER_8259_PORT + ICW4);

    // Initial the slave PIC
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT + ICW4);
    outb(ICW3_SLAVE, SLAVE_8259_PORT + ICW4);
    outb(ICW4, SLAVE_8259_PORT + ICW4);
    
    // set the mask of the master and slave PIC
    outb(master_mask, MASTER_8259_PORT + ICW4);
    outb(slave_mask, SLAVE_8259_PORT + ICW4);

}

/* void enable_irq(void);
 * Inputs: void
 * Return Value: none
 * Function: enable irq */
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    cli();

    // Check the master PIC or the slave PIC
    if (irq_num >= 8) {
        // Mask the slave IRQ
        slave_mask &= (~(0x1 << (irq_num - 8)));
        outb(slave_mask, SLAVE_8259_PORT + ICW4);
        // Mask the master IRQ
        master_mask &= ENABLE_IRQ2;
        outb(master_mask, MASTER_8259_PORT + ICW4);
    }
    else {
        // Mask the master IRQ
        master_mask &= (~(0x1 << irq_num));
        outb(master_mask, MASTER_8259_PORT + ICW4);
    }
    sti();
}

/* void disable_irq(void);
 * Inputs: void
 * Return Value: none
 * Function: disable irq */
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    cli();
    // Check the master PIC or the slave PIC
    if (irq_num >= 8) {
        // Mask the slave IRQ
        slave_mask |= (0x1 << (irq_num - 8));
        outb(slave_mask, SLAVE_8259_PORT + ICW4);
    }
    else {
        // Mask the master IRQ
        master_mask |= (0x1 << irq_num);
        outb(master_mask, MASTER_8259_PORT + ICW4);
    }
    sti();
}

/* void send_eoi(void);
 * Inputs: void
 * Return Value: none
 * Function: send eoi for an IRQ */
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // cli();
    if (irq_num >= 8) {
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
        outb(EOI | 0x2, MASTER_8259_PORT);
    }
    outb(EOI | irq_num, MASTER_8259_PORT);
    // sti();
}

