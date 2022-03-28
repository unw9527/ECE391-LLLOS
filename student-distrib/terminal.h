#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"

extern void clear_buffer(void);


extern int32_t terminal_initial(void);
extern int32_t terminal_close(int32_t fd);
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
