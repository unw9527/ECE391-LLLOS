#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_PORT   0x60
#define KEYBOARD_IRQ    1

#define BACKSPACE       0x08    
#define NEW_LINE        0x0A
#define SPACE           0x20
#define MAX_BUFFER      128
#define MAX_WRITE       8192
#define ORIGIN_X        7


#define F1 0x3B
#define F2 0x3C
#define F3 0x3D

uint8_t enter;
int32_t buffer_index;
volatile uint8_t enter_flag[3];
extern void keyboard_initial(void);
extern void keyboard_handler(void);
extern void echo(uint8_t ascii);


#endif
