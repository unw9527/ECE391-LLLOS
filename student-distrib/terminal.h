#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"

extern void clear_buffer(void);

// Initial the terminal
extern int32_t terminal_initial(void);

// Close the terminal
extern int32_t terminal_close(int32_t fd);

// Read the line buffer to the terminal buffer
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

// Write the buffer to the terminal
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif

