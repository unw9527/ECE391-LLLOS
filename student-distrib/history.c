#include "history.h"
#include "terminal.h"
#include "page.h"
#include "scheduling.h"

static int history_loop = 0; // determine whether the history buffer contains MAX_HISTORY histories
static int retrieve_history_id; // the id of the history we want to print
static int count = 0; // #id we have retrieved


/*
 * void update_history()
 * Input: none
 * Output: none
 * Function: update curr_history_id
 * Side effect: modify history_loop
 */
void update_history(){
    cli();
    if (0 == terminal[curr_terminal].buffer_index){ // if nothing is typed
        retrieve_history_id = curr_history_id - 1;
        sti();
        return;
    }
    // curr_buf_id = 0;
    if (curr_history_id < MAX_HISTORY - 1){
        curr_history_id++;
    }
    else{
        curr_history_id = 0;
        history_loop = 1;
    }
    retrieve_history_id = curr_history_id - 1;
    sti();
}


/*
 * void retrieve_history_up(int32_t start_x, int32_t start_y, uint8_t is_up)
 * Input: the starting coordinate (x and y); 
 *        is_up: whether the last arrow key pressed is up
 * Output: none
 * Function: store the history to line buffer of the current terminal when up is pressed
 * Side effect: clear the history that is already printed
 */
void retrieve_history_up(int32_t start_x, int32_t start_y, uint8_t is_up){
    int j;
    cli();
   
    if (0 == history_loop){
        if (0 > retrieve_history_id) {
            // not allowed to go through the limit
            sti();
            return;
        }
    }

    if (retrieve_history_id == curr_history_id) {
        sti();
        return; // if the next one goes back to where we start, we no longer update retrieve_history_id
    }
    // if (count >= MAX_HISTORY - 1){
    //     sti();
    //     return;
    // }

    while ((terminal[curr_terminal].terminal_x != start_x) || (terminal[curr_terminal].terminal_y != start_y)){
        store_vid_mem(curr_terminal);
        putc(BACK_SPACE, curr_terminal);
        // echo(BACK_SPACE);
        store_vid_mem(running_term);        
    }

    memset((uint8_t*) terminal[curr_terminal].line_buffer, 0, MAX_BUFFER); // clear the line buffer, otherwise will get "no such command" error

    if (is_up == 0){
        retrieve_history_id--;
    }

    for (j = 0; j < MAX_BUFFER; j++){
        if (history_holder[retrieve_history_id][j] == NEW_LINE) break;
        store_vid_mem(curr_terminal);
        putc(history_holder[retrieve_history_id][j], curr_terminal);
        // echo(history_holder[retrieve_history_id][j]);
        store_vid_mem(running_term);
        terminal[curr_terminal].line_buffer[j] = history_holder[retrieve_history_id][j];
    }
    // terminal[curr_terminal].line_buffer[j + 1] = NEW_LINE;
    terminal[curr_terminal].buffer_index = j;
    retrieve_history_id--;
    if ((retrieve_history_id < 0) && history_loop) retrieve_history_id = MAX_HISTORY - 1;
    count++;
    sti();
}



/*
 * void retrieve_history_down(int32_t start_x, int32_t start_y, uint8_t is_up)
 * Input: the starting coordinate (x and y); 
 *        is_up: whether the last arrow key pressed is up
 * Output: none
 * Function: store the history to line buffer of the current terminal when down is pressed
 * Side effect: clear the history that is already printed
 */
void retrieve_history_down(int32_t start_x, int32_t start_y, uint8_t is_up){
    int j;
    cli();
    
    while ((terminal[curr_terminal].terminal_x != start_x) || (terminal[curr_terminal].terminal_y != start_y)){
        store_vid_mem(curr_terminal);
        putc(BACK_SPACE, curr_terminal);
        // echo(BACK_SPACE);
        store_vid_mem(running_term);        
    }

    if (retrieve_history_id == curr_history_id - 1) {
        sti();
        return; // return if already go down to the "blank" state
    } 
    if (0 >= count){
        sti();
        return;
    }

    retrieve_history_id++;
    if (MAX_HISTORY == retrieve_history_id){
        retrieve_history_id = 0;
    }

    memset((uint8_t*) terminal[curr_terminal].line_buffer, 0, MAX_BUFFER); // clear the line buffer, otherwise will get "no such command" error

    if (is_up == 1){
        retrieve_history_id++;
    }

    for (j = 0; j < MAX_BUFFER; j++){
        if (history_holder[retrieve_history_id][j] == NEW_LINE) break;
        store_vid_mem(curr_terminal);
        putc(history_holder[retrieve_history_id][j], curr_terminal);
        // echo(history_holder[retrieve_history_id][j]);
        store_vid_mem(running_term);
        terminal[curr_terminal].line_buffer[j] = history_holder[retrieve_history_id][j];
    }
    terminal[curr_terminal].line_buffer[j + 1] = NEW_LINE;
    terminal[curr_terminal].buffer_index = j + 1;
    count--;

    sti();
}


