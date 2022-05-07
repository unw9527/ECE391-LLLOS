#ifndef _GUI_H
#define _GUI_H
#include "vbe.h"

#define RED_MASK    0xF800
#define GREEN_MASK  0x7E0
#define BLUE_MASK   0x1F

#define ASCII_ZERO  48

#define WHITE       0x00FFFFFF
#define BLACK       0x00000000
#define GRERY       0x00505050

#define TERMINAL_INIT_X 10
#define TERMINAL_INIT_Y 100


extern void show_screen();
extern void draw_background();
extern void draw_terminal(char* terminal_memory, int32_t tid);
extern void draw_string(int x, int y, int8_t* str, uint32_t color, uint32_t back_color);
extern void draw_mouse();
void draw_terminal_window(int x, int y, int w, int h, uint32_t color);
void draw_os_font();
extern void boot_gui();
extern void draw_time_bar();
#endif

