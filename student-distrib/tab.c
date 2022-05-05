#include "tab.h"
#include "terminal.h"
#include "scheduling.h"
#include "page.h"
#include "lib.h"

static uint8_t verylarge[MAX_STRING_SIZE] = "verylargetextwithverylongname.tx";
static uint8_t testprint[MAX_STRING_SIZE] = "testprint";
static uint8_t sigtest[MAX_STRING_SIZE] = "sigtest";
static uint8_t created[MAX_STRING_SIZE] = "created.txt";
static uint8_t counter[MAX_STRING_SIZE] = "counter";
static uint8_t pingpong[MAX_STRING_SIZE] = "pingpong";
static uint8_t syserr[MAX_STRING_SIZE] = "syserr";
static uint8_t cat[MAX_STRING_SIZE] = "cat";
static uint8_t grep[MAX_STRING_SIZE] = "grep";
static uint8_t shell[MAX_STRING_SIZE] = "shell";
static uint8_t fish[MAX_STRING_SIZE] = "fish";
static uint8_t ls[MAX_STRING_SIZE] = "ls";
static uint8_t frame[MAX_STRING_SIZE] = "frame";
static uint8_t frame0[MAX_STRING_SIZE] = "frame0.txt";
static uint8_t frame1[MAX_STRING_SIZE] = "frame1.txt";

/*
 * void delete_to_start_pos(int32_t start_x, int32_t start_y, int i)
 * Input: the starting position of that line, the index of the start of the last word
 * Output: none
 * Function: delete everything and move the cursor to the starting position
 */
void delete_to_start_pos(int32_t start_x, int32_t start_y, int i){
    if ((start_x == 0) && (start_y == 0)){
        start_x = 7;
        start_y = 1;
    }
    // while ((terminal[curr_terminal].terminal_x != start_x) || (terminal[curr_terminal].terminal_y != start_y)){
    while (terminal[curr_terminal].buffer_index > i){
        store_vid_mem(curr_terminal);
        putc(BACK_SPACE, curr_terminal);
        store_vid_mem(running_term); 
        terminal[curr_terminal].buffer_index--;       
    }
}

/*
 * void delete_to_start_pos(int32_t start_x, int32_t start_y)
 * Input: the starting position of that line
 * Output: none
 * Function: delete everything and move the cursor to the starting position
 */
void auto_complete(int i, int32_t start_x, int32_t start_y, int space_detected){
    uint8_t s[MAX_STRING_SIZE];
    int j;
    int idx = 0;
    if (space_detected){
        idx = i + 1;
    }
    if ((terminal[curr_terminal].line_buffer[idx] == 'c') && (terminal[curr_terminal].line_buffer[idx + 1] == 'o')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, counter, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'c') && (terminal[curr_terminal].line_buffer[idx + 1] == 'a')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, cat, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'c') && (terminal[curr_terminal].line_buffer[idx + 1] == 'r')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, created, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 's') && (terminal[curr_terminal].line_buffer[idx + 1] == 'i')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, sigtest, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 's') && (terminal[curr_terminal].line_buffer[idx + 1] == 'y')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, syserr, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 's') && (terminal[curr_terminal].line_buffer[idx + 1] == 'h')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, shell, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'f') && (terminal[curr_terminal].line_buffer[idx + 1] == 'i')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, fish, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'f') && (terminal[curr_terminal].line_buffer[idx + 1] == 'r') && (terminal[curr_terminal].line_buffer[idx + 5] == '0')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, frame0, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'f') && (terminal[curr_terminal].line_buffer[idx + 1] == 'r') && (terminal[curr_terminal].line_buffer[idx + 5] == '1')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, frame1, MAX_STRING_SIZE);
    }
    else if ((terminal[curr_terminal].line_buffer[idx] == 'f') && (terminal[curr_terminal].line_buffer[idx + 1] == 'r')){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, frame, MAX_STRING_SIZE);
    }
    else if (terminal[curr_terminal].line_buffer[idx] == 'g'){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, grep, MAX_STRING_SIZE);
    }
    else if (terminal[curr_terminal].line_buffer[idx] == 'p'){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, pingpong, MAX_STRING_SIZE);
    }
    else if (terminal[curr_terminal].line_buffer[idx] == 't'){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, testprint, MAX_STRING_SIZE);
    }
    else if (terminal[curr_terminal].line_buffer[idx] == 'l'){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, ls, MAX_STRING_SIZE);
    }
    else if (terminal[curr_terminal].line_buffer[idx] == 'v'){
        memset(s, 0, MAX_STRING_SIZE);
        memcpy(s, verylarge, MAX_STRING_SIZE);
    }
    else {return;}


    delete_to_start_pos(start_x + idx, start_y, idx);
    for (j = 0; j < MAX_STRING_SIZE; j++){
        if (s[j] == '\0'){break;}
        store_vid_mem(curr_terminal);
        putc(s[j], curr_terminal);
        store_vid_mem(running_term);   
        terminal[curr_terminal].line_buffer[j + idx] = s[j];
    }
    // j++;
    // store_vid_mem(curr_terminal);
    // putc(' ', curr_terminal);
    // store_vid_mem(running_term); t
    // terminal[curr_terminal].line_buffer[j + idx] = ' ';
    terminal[curr_terminal].buffer_index = j + idx;

    // if (terminal[curr_terminal].line_buffer[0] == (uint8_t) s[0]){
    //     delete_to_start_pos(start_x, start_y, 0);
    //     for (j = 0; j < MAX_STRING_SIZE; j++){
    //         store_vid_mem(curr_terminal);
    //         putc(s[j], curr_terminal);
    //         store_vid_mem(running_term);   
    //         terminal[curr_terminal].line_buffer[j] = s[j];
    //     }
    //     terminal[curr_terminal].buffer_index = j;
    // }
    // if (space_detected && (terminal[curr_terminal].line_buffer[i + 1] == (uint8_t) s[0])){
    //     delete_to_start_pos(start_x + i + 1, start_y, i);
    //     for (j = 0; j < MAX_STRING_SIZE; j++){
    //         store_vid_mem(curr_terminal);
    //         putc(s[j], curr_terminal);
    //         store_vid_mem(running_term);
    //         terminal[curr_terminal].line_buffer[j + i + 1] = s[j];  
    //     }
    //     terminal[curr_terminal].buffer_index = j + i + 1;
    // }
}



/*
 * void press_tab(int32_t start_x, int32_t start_y)
 * Input: the starting position of that line
 * Output: none
 * Function: main function for auto complete
 */
void press_tab(int32_t start_x, int32_t start_y){
    int i;
    int space_detected = 0;
    for (i = 0; i < terminal[curr_terminal].buffer_index; i++){
        if (terminal[curr_terminal].line_buffer[i] == SPACE){
            space_detected = 1;
            break;
        }
    }
    auto_complete(i, start_x, start_y, space_detected);

}


