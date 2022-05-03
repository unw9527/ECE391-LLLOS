#include "mouse.h"
#include "i8259.h"
#include "lib.h"

int32_t mouse_iter = -1;

void mouse_handler()
{
    text_mode_mouse();
    if (!mouse_iter)
        inb(MOUSE_DATA);
    send_eoi(12);
    return;
}

void mouse_init()
{
    uint8_t status;
    uint8_t ack = 0;
    /* Send the COMPAQ to get the status.*/
    outb(0xA8, MOUSE_AUX);
    outb(COMPAQ, MOUSE_AUX);
    status = inb(MOUSE_DATA);
    /* Set bit 1, clear bit 5.*/
    status = (status | 0x2) & CLEAR_BIT5;
    outb(MOUSE_DATA, MOUSE_AUX);
    /* Wait until we can write again.*/
    while ((inb(MOUSE_AUX) & 0x2));
    outb(status, MOUSE_DATA);
    /* Reset the mouse.*/
    // while((inb(MOUSE_AUX) & 0x2));
    // outb(COMMAND_REQUIRE, MOUSE_AUX);
    // while((inb(MOUSE_AUX) & 0x2));
    // outb(MOUSE_RESET, MOUSE_DATA);
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
    inb(MOUSE_DATA);
    enable_irq(12);
}

void text_mode_mouse()
{
    uint8_t mouse_byte1;
    uint8_t mouse_byte2;
    uint8_t mouse_byte3;
    // while ((inb(MOUSE_AUX) & 0x1) != 1);
    // if ((inb(MOUSE_AUX) & COMPAQ) == 0)
    //     return;
    switch (mouse_iter){
        case 0:
            /* Get the value of the mouse.*/
            mouse_byte1 = inb(MOUSE_DATA);
            mouse_iter = (mouse_iter+1) % 3;
            break;
        case 1:
            /* Get the value of the mouse.*/
            mouse_byte2 = inb(MOUSE_DATA);
            mouse_iter = (mouse_iter+1) % 3;
            break;
        case 2:
            mouse_byte3 = inb(MOUSE_DATA);
            /* Update the new cursor.*/
            mouse_update_vid(mouse_byte2, mouse_byte3);
            mouse_iter = (mouse_iter+1) % 3;
            break;
        default:
            mouse_iter = (mouse_iter+1) % 3;
            break;
    }
    return;
}

void mouse_update_vid(uint8_t x, uint8_t y)
{
    return;
}
