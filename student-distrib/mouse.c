// The mouse implementation in this file is obtained from https://wiki.osdev.org/Mouse

#include "mouse.h"
#include "i8259.h"
#include "lib.h"
#include "terminal.h"
#include "page.h"
#include "scheduling.h"
#include "signal.h"
#include "gui.h"
#include "vbe.h"
int32_t mouse_iter = -1;
int32_t x_pos;
int32_t y_pos;

int32_t mouse_timer = 0;

/* void mouse_handler();
 * Inputs: none
 * Return Value: none
 * Function: The mouse handler. */
void mouse_handler()
{
    text_mode_mouse();
    if (!mouse_iter)
        inb(MOUSE_DATA);
    send_eoi(12);
    return;
}

/* void mouse_init();
 * Inputs: none
 * Return Value: none
 * Function: Initialzie the mouse. */
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
    while((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while((inb(MOUSE_AUX) & 0x2));
    outb(MOUSE_RESET, MOUSE_DATA);
    /* Set the sampling rate to 60.*/
    while ((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while ((inb(MOUSE_AUX) & 0x2));
    outb(SET_SAMPLE, MOUSE_DATA);
    while ((inb(MOUSE_AUX) & 0x1) != 1);
    /* Wait until ACK is received.*/
    while (ack != ACK)
        ack = inb(MOUSE_DATA);
    ack = 0;
    while ((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while ((inb(MOUSE_AUX) & 0x2));
    outb(60, MOUSE_DATA);
    while (ack != ACK)
        ack = inb(MOUSE_DATA);
    ack = 0;
    /* Set the resolution.*/
    while ((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while ((inb(MOUSE_AUX) & 0x2));
    outb(RESOLUTION, MOUSE_DATA);
    while ((inb(MOUSE_AUX) & 0x1) != 1);
    /* Wait until ACK is received.*/
    while (ack != ACK)
        ack = inb(MOUSE_DATA);
    ack = 0;
    while ((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while ((inb(MOUSE_AUX) & 0x2));
    outb(0, MOUSE_DATA);
    while (ack != ACK)
        ack = inb(MOUSE_DATA);
    ack = 0;
    /* Check status.*/
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(COMMAND_REQUIRE, MOUSE_AUX);
    // while ((inb(MOUSE_AUX) & 0x2));
    // outb(0xE9, MOUSE_DATA);
    /* Enable the auto packets.*/
    while((inb(MOUSE_AUX) & 0x2));
    outb(COMMAND_REQUIRE, MOUSE_AUX);
    while((inb(MOUSE_AUX) & 0x2));
    outb(ENABLE_PACKET, MOUSE_DATA);
    /* Enable PIC.*/
    inb(MOUSE_DATA);
    enable_irq(12);
}

/* void text_mode_mouse();
 * Inputs: none
 * Return Value: none
 * Function: Implement the 3 packages. */
void text_mode_mouse()
{
    uint8_t mouse_byte1;
    uint8_t mouse_byte2;
    uint8_t mouse_byte3;
    /* Check for mouse interrupt.*/
    if ((inb(MOUSE_AUX) & COMPAQ) == 0)
        return;
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
            mouse_update_vid(mouse_byte1, mouse_byte2, mouse_byte3);
            mouse_click(mouse_byte1);
            mouse_iter = (mouse_iter+1) % 3;
            break;
        default:
            mouse_iter = (mouse_iter+1) % 3;
            break;
    }
    return;
}

/* void mouse_update_vid(uint8_t status, uint8_t x, uint8_t y)
 * Inputs: uint8_t status, uint8_t x, uint8_t y
 * Return Value: none
 * Function: Update the mouse vid */
void mouse_update_vid(uint8_t status, uint8_t x, uint8_t y)
{
    int8_t sign_x;
    int8_t sign_y;
    if (x == 0 && y == 0)
        return;
    /* Check overflow.*/
    if ((status & X_OVERFLOW) || (status & Y_OVERFLOW))
        return;
    /*Change the color back to white.*/
    // store_vid_mem(curr_terminal);
    // if (*(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1)) == '#')
    //     *(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1)) = ' ';
    // *(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1) + 1) = ATTRIB;

    /* Update the mouse position.*/
    /* Check for neg x.*/
    if (status & X_SIGN){
        sign_x = (int8_t) x;
        x_pos += (int32_t)sign_x;
        if (x_pos < 0)
            x_pos = 0;
    }
    else{
        x_pos += (int32_t)x;
        if (x_pos >= BG_WIDTH)
            x_pos = BG_WIDTH - 1;
    }
    /* Check for neg y.*/
    if (status & Y_SIGN){
        sign_y = (int8_t) y;
        y_pos -= (int32_t)sign_y;
        if (y_pos >= BG_HEIGHT)
            y_pos = BG_HEIGHT - 1;
    }
    else{
        y_pos -= (int32_t)y;
        if (y_pos < 0)
            y_pos = 0;
    }

    /* Update the new green position.*/
    // if (*(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1)) == ' ')
    //     *(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1)) = '#';
    // *(uint8_t *)(video_mem + ((x_pos + y_pos * NUM_COLS) << 1) + 1) = GREEN;
    return;
}

/* void mouse_click();
 * Inputs: none
 * Return Value: none
 * Function: Update the signal accordingly. */
void mouse_click(uint8_t status)
{
    /* Check overflow.*/
    if ((status & X_OVERFLOW) || (status & Y_OVERFLOW))
        return;
    /* Check for right click.*/
    if ((status & RIGHT_BUTTON) == 0 && (status & LEFT_BUTTON) == 0)
        return;
    if (status & RIGHT_BUTTON){
        /* Update the USER1 signal.*/
        signal_update(USER1);
    }
    else if (status & LEFT_BUTTON){
        /* Update the USER2 signal.*/
        signal_update(USER2);
    }
    return;
}
