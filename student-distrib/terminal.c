#include "terminal.h"
#include "process.h"
#include "page.h"
#include "x86_desc.h"
#include "syscall.h"
#include "scheduling.h"
/* void clear_buffer(void)
 * Inputs: void
 * Return Value: none
 * Function: clear the terminal buffer */
void clear_buffer(void) {
    int i;                      // loop indice
    terminal[curr_terminal].buffer_index = 0;           // reset the buffer index
    for (i = 0; i < MAX_BUFFER; i++) {
        terminal[curr_terminal].line_buffer[i] = NULL;      // clear the content of line buffer
    }
}


/* int32_t terminal_init(void)
 * Inputs: void
 * Return Value: none
 * Function: Initial the terminal */
void terminal_init(void) {

    clear();
    reset_cursor();
    clear_buffer();
    int i;
	for (i = 0; i < MAX_TERMINAL; i++) {
        //  terminal[i].enter_flag = 0;
         terminal[i].terminal_x = 0;
         terminal[i].terminal_y = 0;
         terminal[i].buffer_index = 0;
         terminal[i].terminal_prog_count = 0;
	}
    terminal[0].vid_mem = TERMINAL1_ADDRESS;
    terminal[1].vid_mem = TERMINAL2_ADDRESS;
    terminal[2].vid_mem = TERMINAL3_ADDRESS;
	return;
}


/* int32_t terminal_open(void)
 * Inputs: void
 * Return Value: 0
 * Function: Initial the terminal */
int32_t terminal_open(const uint8_t* filename) {
    clear();                        // clear the screen
    reset_cursor();                 // reset the cursor
    clear_buffer();                 // clear the buffer
    return 0;
}

/* int32_t terminal_close(int32_t fd)
 * Inputs: fd -- unused
 * Return Value: 0
 * Function: close the terminal */
int32_t terminal_close(int32_t fd) {
    clear_buffer();
    return 0;
}

/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: fd -- unused
 *         buf -- stored the line buffer
 *         nbytes -- unused
 * Return Value: the size of the line buffer
 * Function: store the line buffer to the terminal */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int i;                                  // loop indice
    int size;                               // the size of the current buffer
    int8_t* buf1 = (int8_t *)buf;           // transform the type of the buffer
    if (NULL == buf1) 
        return -1;
    while(!enter_flag[running_term]);                          // wait for enter to press
    cli();
    size = terminal[running_term].buffer_index;
    if (size > MAX_BUFFER)                  // only support the number smaller than the max buffer
        size = MAX_BUFFER;
    enter_flag[running_term] = 0;

    for (i = 0; i < MAX_BUFFER; i++){       // store the line buffer to the buf
        if (i < terminal[running_term].buffer_index) 
            buf1[i] = terminal[running_term].line_buffer[i];
        terminal[running_term].line_buffer[i] = NULL;
    }
    buf1[terminal[running_term].buffer_index] = NEW_LINE;
    terminal[running_term].buffer_index = 0;
    sti();
    return size+1;
}

/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: fd -- unused
 *         buf -- stored the line buffer
 *         nbytes -- the size of the line buffer
 * Return Value: the size of the line buffer
 * Function: write the line buffer to the terminal */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int8_t* buf1 = (int8_t *)buf;
    int i;
    if ((nbytes > MAX_WRITE) || (NULL == buf1))
        return -1;
    cli();
    for (i = 0; i < nbytes; i++) {        // print the content of the buf
        putc(buf1[i], running_term);
    }
    sti();
    return nbytes;
}

/* int32_t switch_terminal(int32_t term_id)
 * Inputs: term_id -- the terminal id to be switched
 * Return Value: none
 * Function: switch the terminal */
void switch_terminal(int32_t term_id) {   
    int32_t prev_terminal;

    if( term_id < 0 || term_id > 2)         // check whether the terminal id is valid
        return;

    prev_terminal = curr_terminal;          // store the previous terminal id

    if (term_id == prev_terminal)           // if the terminal id equals to the previos terminal, just return
        return;

    curr_terminal = term_id;                // switch to current terminal

    store_vid_mem(curr_terminal);           // store the video memory to the current terminal

    // Copy the memory between current terminal and previous terminal
    memcpy((void*) terminal[prev_terminal].vid_mem, (const void*)VIDEO, VIDEO_MEM_SIZE);      

    memcpy((void*)VIDEO, (const void*) terminal[curr_terminal].vid_mem, VIDEO_MEM_SIZE);    
    
    // Move the cursor to the current terminal
    move_cursor(curr_terminal);

    return;
}



int32_t (*read_terminal_pt) (int32_t, void*, int32_t) = terminal_read;
int32_t (*open_terminal_pt) (const uint8_t*) = terminal_open;
int32_t (*close_terminal_pt) (int32_t) = terminal_close;
int32_t (*write_terminal_pt) (int32_t, const void*, int32_t) = terminal_write;


