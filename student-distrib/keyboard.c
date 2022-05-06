// The keycode information in this file is obtained from https://wiki.osdev.org/Keyboard

#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "tests.h"
#include "terminal.h"
#include "page.h"
#include "scheduling.h"
#include "history.h"
#include "signal.h"
#include "tab.h"

// the key flag
uint8_t caps;
uint8_t shift;
uint8_t alt;
uint8_t ctrl;
uint8_t normal_key; // any keys that can be displayed
int32_t tab;

typedef struct key                                                              /* The first entry is the scan code and the second is the ascii.*/
{
    uint8_t scan;
    uint8_t ascii;
} key_t;

typedef struct ascii                                                              /* The first entry is the scan code and the second is the ascii.*/
{
    uint8_t ascii1;
    uint8_t ascii2;
} ascii_t;

static key_t key_map[51] =                                                     /* The little case letter and numbers.*/
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
    {0x2C, 0x7A},
    
    {0x29, 0x60},       /* backtick: `  */
    {0x0C, 0x2D},       /* dash: -      */
    {0x0D, 0x3D},       /* equal: =     */
    {0x1A, 0x5B},       /* braket_l: [  */
    {0x1B, 0x5D},       /* braket_r: ]  */
    {0x2B, 0x5C},       /* backslash: \ */
    {0x27, 0x3B},       /* semicolon: ; */
    {0x28, 0x27},       /* quote: '     */
    {0x33, 0x2C},       /* comma: ,     */
    {0x34, 0x2E},       /* period: .    */
    {0x35, 0x2F},       /* fw_slash: /  */
    {0x39, 0x20},       /* space        */
    {0x0F, 0x20},       /* tab          */
    {0x0E, 0x08},       /* backspace    */
};

static ascii_t shift_table[24] = 
{
    {0x30, 0x29},       /* 0 to )       */
    {0x31, 0x21},       /* 1 to !       */
    {0x32, 0x40},       /* 2 to @       */
    {0x33, 0x23},       /* 3 to #       */
    {0x34, 0x24},       /* 4 to $       */
    {0x35, 0x25},       /* 5 to %       */
    {0x36, 0x5E},       /* 6 to ^       */
    {0x37, 0x26},       /* 7 to &       */
    {0x38, 0x2A},       /* 8 to *       */
    {0x39, 0x28},       /* 9 to (       */
    {0x60, 0x7E},       /* ` to ~       */
    {0x2D, 0x5F},       /* - to _       */
    {0x3D, 0x2B},       /* = to +       */
    {0x5B, 0x7B},       /* [ to {       */
    {0x5D, 0x7D},       /* ] to }       */
    {0x5C, 0x7C},       /* \ to |       */
    {0x3B, 0x3A},       /* ; to :       */
    {0x27, 0x22},       /* ' to "       */
    {0x2C, 0x3C},       /* , to <       */
    {0x2E, 0x3E},       /* . to >       */
    {0x2F, 0x3F}        /* / to ?       */
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
    shift = 0; 
    enter = 0;                                                                 /* To indicate the shift pressed.*/
    enter_flag[0] = 0;
    enter_flag[1] = 0;
    enter_flag[2] = 0;
    curr_history_id = 0;
    normal_key = 0;
    tab = 0;
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
    for (i = 0; i < 51; i++) {                                                   /* There are 36 letters for normal case.*/
        if (key_map[i].scan == scan_code)
            return key_map[i].ascii;
    }
    return 0;
}

uint8_t shift_ascii(uint8_t ascii_value) {
    int i;
    for (i = 0; i < 24; i++) {                                                   /* There are 36 letters for normal case.*/
        if (shift_table[i].ascii1 == ascii_value)
            return shift_table[i].ascii2;
    }
    return 0;
}

void echo(uint8_t ascii_value) {
    uint8_t ascii;
    if (ascii_value == 0 || (ascii_value == BACKSPACE && terminal[curr_terminal].buffer_index == 0))
        return;
    ascii = ascii_value;
    if ((ascii_value >= 0x61) && (ascii_value <= 0x7A)) {
        if ((shift == 2? 1:shift) != caps)
            ascii -= 0x20;
    }
    else if (shift)
            ascii = shift_ascii(ascii);
    putc(ascii, curr_terminal);
    // determine whether the backspace is pressed
    if (ascii == BACKSPACE) {
        if (terminal[curr_terminal].buffer_index > 0) {                                 // decrement the line buffer
            if (terminal[curr_terminal].buffer_index <= MAX_BUFFER){
                terminal[curr_terminal].line_buffer[terminal[curr_terminal].buffer_index - 1] = NEW_LINE;
            }
            terminal[curr_terminal].buffer_index--;
            if (terminal[curr_terminal].buffer_index == 0) {
                update_retri_id();
            }
        }
    }
    else {
        if (terminal[curr_terminal].buffer_index < MAX_BUFFER - 1) {                    // increment the buffer_index and store the line buffer
            terminal[curr_terminal].line_buffer[terminal[curr_terminal].buffer_index] = ascii;
            terminal[curr_terminal].line_buffer[terminal[curr_terminal].buffer_index + 1] = NEW_LINE;
            terminal[curr_terminal].buffer_index++;
        }
        else {
            terminal[curr_terminal].buffer_index++;
        }
    }
    sti();
}


/* keyboard_handler
 * 
 * The keyboard handler
 * Inputs: none
 * Outputs: none
 * Side Effects: none
 */
void keyboard_handler(void) {
    uint8_t scan_code = inb(KEYBOARD_PORT);
    uint8_t ascii_value;
    cli();
    switch (scan_code)
    {
    case 0x3A:          // CAPS LOCK pressed
        caps = caps ^ 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0xBA:          // CAPS LOCK released
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x1D:          // ctrl pressed
        ctrl = 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x9D:          // ctrl released
        ctrl = 0;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x2A:          // left shift pressed
        shift += 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    
    case 0xAA:          // left shift released
        shift -= 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x36:          // right shift pressed
        shift += 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    
    case 0xB6:          // right shift released
        shift -= 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x1C:          // Enter pressed
        normal_key = 0;
        ascii_value = 0x0A;
        enter_flag[curr_terminal] = 1;
        store_vid_mem(curr_terminal);
        putc('\n', curr_terminal);
        store_vid_mem(running_term);
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    case 0x9C:         // Enter released
        // update_history();
        store_vid_mem(curr_terminal);
        store_vid_mem(running_term);
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;

    case 0x38:
        alt = 1;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    case 0xB8:
        alt = 0;
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    case 0x48: // up arrow pressed 
        send_eoi(KEYBOARD_IRQ);
        if (0 == terminal[curr_terminal].buffer_index){
            normal_key = 0;
        }
        if (0 == normal_key){
            retrieve_history_up();
        }
        sti();
        return;
    case 0xC8: // up arrow released 
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    case 0x50: // down arrow pressed
        send_eoi(KEYBOARD_IRQ);
        
        if (0 == normal_key) {
            retrieve_history_down();
        }

        sti();
        return;
    case 0xD0: // down arrow released
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    case 0x0F:          // tab is pressed
        send_eoi(KEYBOARD_IRQ);
        if (0 == tab){
            tab = 1;
            press_tab();
        }
        sti();
        return;
    case 0x8F:          // tab is released
        send_eoi(KEYBOARD_IRQ);
        tab = 0;
        sti();
        return;
    default:
        ascii_value = key_to_ascii(scan_code);
        if (((32 <= ascii_value) && (127 >= ascii_value)) || ((0 != terminal[curr_terminal].buffer_index) && (BACK_SPACE == ascii_value)))  normal_key = 1;
    }
    
    if(alt && !shift) {
        switch(scan_code)
        {
            case F1:
                switch_terminal(0);
                store_vid_mem(running_term);
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;

            case F2:
                switch_terminal(1);
                store_vid_mem(running_term);
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;

            case F3:
                switch_terminal(2);
                store_vid_mem(running_term);
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;

            default:
                send_eoi(KEYBOARD_IRQ);
                sti();
                return;
        }

    }

    store_vid_mem(curr_terminal);
    /* ctrl + l */
    if (ctrl & (ascii_value == 0x6C)) {
        send_eoi(KEYBOARD_IRQ);
        sti();   
        clear();                            // clear the creen
        reset_cursor();     
        buffer_index = 0;                   // reset the buffer index
        enter = 0;  
        sti();                         
        return;                            
    }

    /* ctrl + space */
    if (ctrl & (scan_code == 0x39)) {
        send_eoi(KEYBOARD_IRQ);
        sti();
        refresh_and_test();                 // change test
        buffer_index = 0;                   // reset the buffer index
        enter = 0;
        sti();
        return;
    }

    /* ctrl + c */
    if (ctrl & (ascii_value == 0x63)){
        send_eoi(KEYBOARD_IRQ);
        signal_update(INTERRUPT);
        buffer_index = 0;
        enter = 0;
        sti();
        return;
    }


    // echo the key
    echo(ascii_value);
    store_vid_mem(running_term);
    send_eoi(KEYBOARD_IRQ);
}
