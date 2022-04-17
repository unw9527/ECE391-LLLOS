#include "terminal.h"
#include "process.h"
#include "page.h"
#include "x86_desc.h"
#include "syscall.h"
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

void terminal_init(void) {

    clear();
    reset_cursor();
    clear_buffer();
    curr_terminal = 0;
    curr_terminal = 0;
    int i;
	for (i = 0; i < MAX_TERMINAL; i++) {
         terminal[i].terminal_x = 0;
         terminal[i].terminal_y = 0;
         terminal[i].buffer_index = 0;
         terminal[i].terminal_prog_count = 0;
	}

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
    while(!enter);                          // wait for enter to press
    size = terminal[curr_terminal].buffer_index;
    if (size > MAX_BUFFER)                  // only support the number smaller than the max buffer
        size = MAX_BUFFER;
    enter = 0;

    for (i = 0; i < MAX_BUFFER; i++){       // store the line buffer to the buf
        if (i < terminal[curr_terminal].buffer_index)
            buf1[i] = terminal[curr_terminal].line_buffer[i];
        terminal[curr_terminal].line_buffer[i] = NULL;
    }
    terminal[curr_terminal].buffer_index = 0;
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
    sti();
    if ((nbytes > MAX_BUFFER - 1) || (NULL == buf1))
        return -1;
    int i;
    for (i = 0; i < nbytes; i++)        // print the content of the buf
        putc(buf1[i], curr_terminal);
    return nbytes;
}

void switch_terminal(int32_t term_id)
{   
    printf("Terminal %d\n", term_id);
    int32_t prev_terminal;

    if( term_id < 0 || term_id > 2)
        return;

    prev_terminal = curr_terminal;

    if (term_id == prev_terminal)
        return;
    clear();

    curr_terminal = term_id;
    
    if (terminal[curr_terminal].terminal_prog_count == 0) {
        sys_execute((uint8_t*)"shell");
    }
    else {
        process_counter = terminal[curr_terminal].prog_array[terminal[curr_terminal].terminal_prog_count-1];
        // swap_page(process_counter);
        // tss.esp0 = STACK_BASE - 4 * KERNEL_STACK * process_counter - 4;
        // asm volatile("                                                \n\
        //     pushw $0                                                  \n\
        //     pushw %0                                                  \n\
        //     pushl %1                                                  \n\
        //     pushfl                                                    \n\
        //     popl %%ecx                                                \n\
        //     orl $0x200,%%ecx                                          \n\
        //     pushl %%ecx                                               \n\
        //     pushw $0                                                  \n\
        //     pushw %2                                                  \n\
        //     pushl %3                                                  \n\
        //     movw %0, %%cx                                             \n\
        //     movw %%cx, %%ds                                           \n\
        //     iret                                                      \n\
        //     "                                                           \
        // : /* no output*/                                                \
        // : "g"((USER_DS)), "g"((USER_SPACE_ESP)), "g"((USER_CS)), "g"(PCB_array[NUM_PROCESS-1-process_counter].thread_info.entry_point)\
        // : "memory", "%ecx"/* no register modification*/                 \
        // );
    }
    
    move_cursor(curr_terminal);

    return;
}



int32_t (*read_terminal_pt) (int32_t, void*, int32_t) = terminal_read;
int32_t (*open_terminal_pt) (const uint8_t*) = terminal_open;
int32_t (*close_terminal_pt) (int32_t) = terminal_close;
int32_t (*write_terminal_pt) (int32_t, const void*, int32_t) = terminal_write;
