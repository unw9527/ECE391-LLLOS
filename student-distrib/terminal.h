#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "process.h"

#define MAX_TERMINAL        3
#define TERMINAL1_ADDRESS   0xB9000
#define TERMINAL2_ADDRESS   0xBA000
#define TERMINAL3_ADDRESS   0xBB000
#define VIDEO_MEM_SIZE      4096


extern void clear_buffer(void);

extern void terminal_init(void);

// Initial the terminal
extern int32_t terminal_open(const uint8_t* filename);

// Close the terminal
extern int32_t terminal_close(int32_t fd);

// Read the line buffer to the terminal buffer
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

// Write the buffer to the terminal
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

extern void switch_terminal(int32_t term_id);

typedef struct terminal {
         int32_t    terminal_x;
         int32_t    terminal_y;

         int32_t    vid_mem;
         int32_t    terminal_prog_count;
         int32_t    prog_array[6];
         volatile uint8_t    line_buffer[MAX_BUFFER];
         volatile int32_t    buffer_index;
} terminal_t;

terminal_t  terminal[MAX_TERMINAL];
int32_t     curr_terminal; // the terminal that is being displayed
int32_t     total_prog_count;  
#endif

