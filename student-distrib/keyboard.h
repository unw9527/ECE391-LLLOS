#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_PORT 0x60
#define KEYBOARD_IRQ  1

extern void keyboard_initial(void);
extern void keyboard_handler(void);

#endif
