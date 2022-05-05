#include "history.h"
#include "terminal.h"
#include "page.h"
#include "scheduling.h"

static int history_loop = 0; // determine whether the history buffer contains MAX_HISTORY histories
static int retrieve_history_id; // the id of the history we want to print
static int count = 0; // #id we have retrieved


/*
 * void update_history(int32_t is_max_history)
 * Input: int32_t is_max_history: 1 if curr_history_id = MAX_HISTORY; 0 otherwise
 * Output: none
 * Function: update curr_history_id
 * Side effect: modify history_loop
 */
void update_history(int32_t is_max_history){
    cli();
    if (is_max_history){
        curr_history_id = 0;
        history_loop = 1;
    }
    else{
        if (0 == terminal[curr_terminal].buffer_index){ // if nothing is typed
            retrieve_history_id = curr_history_id;
            sti();
            return;
        }
        curr_history_id++;
        retrieve_history_id = curr_history_id;
        terminal[curr_terminal].buffer_index = 0;
        count = 0;
    }
    sti();
}


/*
 * void retrieve_history_up()
 * Input: none
 * Output: none
 * Function: store the history to line buffer of the current terminal when up is pressed
 * Side effect: clear the history that is already printed
 */
void retrieve_history_up(){
    int j;
    cli();
   
    retrieve_history_id--;

    if (0 == history_loop){
        if (0 > retrieve_history_id) {
            // not allowed to go through the limit
            retrieve_history_id++;
            sti();
            return;
        }
    }
    if (history_loop){
        if (count == MAX_HISTORY) {
            retrieve_history_id++;
            sti();
            return; // if the next one goes back to where we start, we no longer update retrieve_history_id
        }
        if (-1 == retrieve_history_id){
            retrieve_history_id = MAX_HISTORY - 1;
        }
    }
    
    while (terminal[curr_terminal].buffer_index > 0){
        store_vid_mem(curr_terminal);
        putc(BACK_SPACE, curr_terminal);
        store_vid_mem(running_term);  
        terminal[curr_terminal].buffer_index--;
    }
    
    

    memset((uint8_t*) terminal[curr_terminal].line_buffer, 0, MAX_BUFFER); // clear the line buffer, otherwise will get "no such command" error


    for (j = 0; j < MAX_BUFFER; j++){
        if (history_holder[retrieve_history_id][j] == NEW_LINE) break;
        store_vid_mem(curr_terminal);
        putc(history_holder[retrieve_history_id][j], curr_terminal);
        store_vid_mem(running_term);
        terminal[curr_terminal].line_buffer[j] = history_holder[retrieve_history_id][j];
    }
    terminal[curr_terminal].buffer_index = j;
    
    count++;
    sti();
}



/*
 * void retrieve_history_down()
 * Input: none
 * Output: none
 * Function: store the history to line buffer of the current terminal when down is pressed
 * Side effect: clear the history that is already printed
 */
void retrieve_history_down(){
    int j;
    cli();
    
    if (count > 0){
        retrieve_history_id++;
        count--;
        memset((uint8_t*) terminal[curr_terminal].line_buffer, 0, MAX_BUFFER); // clear the line buffer, otherwise will get "no such command" error

        while (terminal[curr_terminal].buffer_index > 0){
            store_vid_mem(curr_terminal);
            putc(BACK_SPACE, curr_terminal);
            store_vid_mem(running_term);       
            terminal[curr_terminal].buffer_index--; 
        }
    }

    if (0 == count) {
        sti();
        return; // return if already go down to the "blank" state
    } 

    if (MAX_HISTORY == retrieve_history_id){
        retrieve_history_id = 0;
    }

    for (j = 0; j < MAX_BUFFER; j++){
        if (history_holder[retrieve_history_id][j] == NEW_LINE) break;
        store_vid_mem(curr_terminal);
        putc(history_holder[retrieve_history_id][j], curr_terminal);
        store_vid_mem(running_term);
        terminal[curr_terminal].line_buffer[j] = history_holder[retrieve_history_id][j];
    }
    terminal[curr_terminal].buffer_index = j;

    sti();
}

/*
 * void update_retri_id()
 * Input: none
 * Output: none
 * Function: increment retrieve_history_id
 */
void update_retri_id(){
    retrieve_history_id++;
}

