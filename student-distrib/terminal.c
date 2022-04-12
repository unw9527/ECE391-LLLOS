#include "terminal.h"

/* void clear_buffer(void)
 * Inputs: void
 * Return Value: none
 * Function: clear the terminal buffer */
void clear_buffer(void) {
    int i;                      // loop indice
    buffer_index = 0;           // reset the buffer index
    for (i = 0; i < MAX_BUFFER; i++) {
        line_buffer[i] = NULL;      // clear the content of line buffer
    }
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
    while(!enter);                          // wait for enter to press
    size = buffer_index;
    if (size > MAX_BUFFER)                  // only support the number smaller than the max buffer
        size = MAX_BUFFER;
    enter = 0;

    for (i = 0; i < MAX_BUFFER; i++){       // store the line buffer to the buf
        if (i < buffer_index)
            buf1[i] = line_buffer[i];
        line_buffer[i] = NULL;
    }
    buffer_index = 0;
    return size;
}

/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: fd -- unused
 *         buf -- stored the line buffer
 *         nbytes -- the size of the line buffer
 * Return Value: the size of the line buffer
 * Function: write the line buffer to the terminal */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    int8_t* buf1 = (int8_t *)buf;

    if ((nbytes > MAX_BUFFER - 1) || (NULL == buf1))
        return -1;
    int i;
    for (i = 0; i < nbytes; i++)        // print the content of the buf
        putc(buf1[i]);
    return nbytes;
}

int32_t (*read_terminal_pt) (int32_t, void*, int32_t) = terminal_read;
int32_t (*open_terminal_pt) (const uint8_t*) = terminal_open;
int32_t (*close_terminal_pt) (int32_t) = terminal_close;
int32_t (*write_terminal_pt) (int32_t, const void*, int32_t) = terminal_write;
