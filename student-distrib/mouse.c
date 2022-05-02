#include "mouse.h"
#include "i8259.h"
#include "lib.h"

void mouse_handler()
{
    send_eoi(12);
    return;
}

void mouse_init()
{
    uint8_t status;
    uint8_t ack = 0;
    /* Send the COMPAQ to get the status.*/
    outb(COMPAQ, MOUSE_AUX);
    status = inb(MOUSE_DATA);
    /* Set bit 1, clear bit 5.*/
    status = (status | 0x6) & CLEAR_BIT5;
    outb(MOUSE_DATA, MOUSE_AUX);
    /* Wait until we can write again.*/
    while ((inb(MOUSE_AUX) & 0x2));
    outb(status, MOUSE_DATA);
    /* Reset the mouse.*/
    while((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while((inb(MOUSE_AUX) & 0x2));
    outb(MOUSE_RESET, MOUSE_DATA);
    /* Set the sampling rate to 60.*/
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(COMMAND_REQUIRE, MOUSE_AUX);
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(SET_SAMPLE, MOUSE_DATA);
    // while ((inb(MOUSE_AUX) & 0x1) != 1);
    /* Wait until ACK is received.*/
    // while (ack != ACK)
    //     ack = inb(MOUSE_DATA);
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(COMMAND_REQUIRE, MOUSE_AUX);
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(60, MOUSE_DATA);
    /* Enable the auto packets.*/
    while((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while((inb(MOUSE_AUX) & 0x2));
    outb(ENABLE_PACKET, MOUSE_DATA);
    /* Enable PIC.*/
    enable_irq(12);
}
