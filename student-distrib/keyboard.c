#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "tests.h"

uint8_t caps;
uint8_t shift;
uint8_t alt;
uint8_t ctrl;


typedef struct key                                                              /* The first entry is the scan code and the second is the ascii.*/
{
    uint8_t scan;
    uint8_t ascii;
} key_t;

static key_t key_map[36] =                                                     /* The little case letter and numbers.*/
{
    /* 0 - 9 */
    {0x0B, 0x30}, {0x02, 0x31}, {0x03, 0x32}, {0x04, 0x33}, {0x05, 0x34}, 
    {0x06, 0x35}, {0x07, 0x36}, {0x08, 0x37}, {0x09, 0x38}, {0x0A, 0x39}, 

    /* A - Z */
    {0x1E, 0x61}, {0x30, 0x62}, {0x2E, 0x63}, {0x20, 0x64}, {0x12, 0x65}, 
    {0x21, 0x66}, {0x22, 0x67}, {0x23, 0x68}, {0x17, 0x69}, {0x24, 0x6A}, 
    {0x25, 0x6B}, {0x26, 0x6C}, {0x32, 0x6D}, {0x31, 0x6E}, {0x18, 0x6F}, 
    {0x19, 0x70}, {0x10, 0x71}, {0x13, 0x72}, {0x1F, 0x73}, {0x14, 0x74}, 
    {0x16, 0x75}, {0x2F, 0x76}, {0x11, 0x77}, {0x2D, 0x78}, {0x15, 0x79}, 
    {0x2C, 0x7A}

};
/* key_to_ascii
 * 
 * Initial the keyboard
 * Inputs: scan_code
 * Outputs: 0
 * Side Effects: none
 */
void keyboard_initial(void) {
    ctrl = 0;                                                                   /* To decide whether control is entered.*/
    caps = 0;                                                                   /* To indicate the capital case. */
    alt  = 0;                                                                   /* To indicate the alt.*/
    shift = 0;                                                                  /* To indicate the shift pressed.*/
    enable_irq(KEYBOARD_IRQ);
}

/* key_to_ascii
 * 
 * Change the key to ascii.
 * Inputs: scan_code
 * Outputs: 0 or ascii
 * Side Effects: none
 */
uint8_t key_to_ascii(uint8_t scan_code) {
    int i;
    for (i = 0; i < 36; i++) {                                                   /* There are 36 letters for normal case.*/
        if (key_map[i].scan == scan_code)
            return key_map[i].ascii;
    }
    return 0;
}

/* keyboard_handler
 * 
 * The keyboard handler
 * Inputs: none
 * Outputs: none
 * Side Effects: none
 */
void keyboard_handler(void) {
    cli();
    uint8_t scan_code = inb(KEYBOARD_PORT);
    uint8_t ascii_value;
    
    switch (scan_code)
    {
    case 0x3A:          // CAPS LOCK pressed
        caps = (caps == 1)? 0 : 1;
        send_eoi(KEYBOARD_IRQ);
        return;
    case 0xBA:          // CAPS LOCK released
        send_eoi(KEYBOARD_IRQ);
        return;
    case 0x1C:          // Enter pressed
        refresh_and_test();
        send_eoi(KEYBOARD_IRQ);
        return;
    default:
        break;
    }

    ascii_value = key_to_ascii(scan_code);

    if ((caps == 1) && (ascii_value >= 0x61) && (ascii_value <= 0x7A))                  /* To change to the capital letters.*/
        ascii_value -= 0x20;
    if(ascii_value != 0)                                                                /* Print the ascii value.*/
        putc(ascii_value);
    
    send_eoi(KEYBOARD_IRQ);
    sti();
}
