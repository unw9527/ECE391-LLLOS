#ifndef _HISTORY_H
#define _HISTORY_H

#include "terminal.h"
#include "lib.h"

#define MAX_HISTORY     10

uint8_t history_holder[MAX_HISTORY][MAX_BUFFER];
int curr_buf_id; // only allow the user to press up arrow when curr_buf_id = 0
int curr_history_id;

extern void update_history();
extern void retrieve_history_up(int32_t start_x, int32_t start_y);
extern void retrieve_history_down(int32_t start_x, int32_t start_y, uint8_t is_prev_up);

#endif

