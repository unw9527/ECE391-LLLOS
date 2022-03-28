#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_PORT 0x60
#define KEYBOARD_IRQ  1

#define NEW_LINE    0x0A
#define MAX_BUFFER 128

uint8_t enter;
uint8_t line_buffer[MAX_BUFFER];
int32_t buffer_index;

extern void keyboard_initial(void);
extern void keyboard_handler(void);


#endif
