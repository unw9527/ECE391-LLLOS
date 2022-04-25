#ifndef _HISTORY_H
#define _HISTORY_H

#include "terminal.h"
#include "lib.h"

#define MAX_HISTORY     20

uint8_t history_holder[MAX_HISTORY][MAX_BUFFER];
int curr_history_id;

extern void update_history(int32_t is_max_history);
extern void retrieve_history_up(int32_t start_x, int32_t start_y);
extern void retrieve_history_down(int32_t start_x, int32_t start_y);

#endif

