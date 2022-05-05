#include "gui.h"
#include "desktop_color.h"
#include "os_name_font.h"
#include "boot_font.h"
#include "uiuc.h"
#include "transparent.h"
#include "cloud_terminal.h"
#include "mouse_img.h"
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
#include "font.h"
#include "terminal.h"
#include "scheduling.h"
#include "page.h"
#include "time.h"
#include "mouse.h"

// References: https://docs.microsoft.com/en-us/windows/win32/directshow/working-with-16-bit-rgb (color encoding)

int32_t last_x = 0, last_y = 0;
uint32_t mouse_initial = 0;
uint32_t mouse_buffer[22][34];

uint32_t* qemu_memory = (uint32_t *) QEMU_BASE_ADDR;
int box = 1;
int back = 0;
int col = 0x8B00FF;

/*
 * void sleep(int32_t time)
 * Input: time - time to wait
 * Return value: none
 */
void sleep(int32_t time) {
    int32_t k = 0;
    while (k < time) {
        k++;
    }
}

/*
 * void draw_transparent()
 * Input: none
 * Return value: none
 * Function: draw transparent text
 */
void draw_transparent() {
    int i;
    int j;
    for (i = 0; i < 192; i++) {
        for (j = 0; j < 256; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((tp[j + i * 256] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((tp[j + i * 256] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((tp[j + i * 256] & BLUE_MASK) << 3) & 0xFF);
            *(uint32_t *)(qemu_memory + 4 * j + 4 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 1 + 4 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 2 + 4 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 3 + 4 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + (4 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 1 + (4 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 2 + (4 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 3 + (4 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + (4 * i + 2) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 1 + (4 * i + 2) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 2 + (4 * i + 2) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 3 + (4 * i + 2) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + (4 * i + 3) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 1 + (4 * i + 3) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 2 + (4 * i + 3) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 4 * j + 3 + (4 * i + 3) * BG_WIDTH) = rgb;
        }
    }
}


/*
 * void draw_uiuc()
 * Input: none
 * Return value: none
 * Function: draw the UIUC background for boot animation
 */
void draw_uiuc() {
    int i,j;
    for (i = 0; i < 130; i++) {
        for (j = 0; j < 420; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((uiuc[j + i * 420] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((uiuc[j + i * 420] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((uiuc[j + i * 420] & BLUE_MASK) << 3) & 0xFF);
            if (rgb >= 0x00F85CF8)
                continue;
            *(uint32_t *)(qemu_memory + j + 300 + (i + 400) * BG_WIDTH) = rgb;
        }
        sleep(0x8FFFFF);
    }
    sleep(0x8FFFFF);
}

/*
 * void draw_boot_font()
 * Input: none
 * Return value: none
 * Function: draw the boot text for boot animation
 */
void draw_boot_font() {
    int i,j;
    for (j = 0; j < 600; j++) {
        for (i = 0; i < 80; i++) {
            uint32_t rgb = BLACK;
            rgb |= ((((boot_font[j + i * 600] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((boot_font[j + i * 600] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((boot_font[j + i * 600] & BLUE_MASK) << 3) & 0xFF);
            if (rgb < 0x00111111)
                continue;
            *(uint32_t *)(qemu_memory + j + 200 + (i + 200) * BG_WIDTH) = 0x00000055;
        }
        sleep(0xFFFFF);
    }
    draw_uiuc();
}

/*
 * void draw_background()
 * Input: none
 * Return value: none
 * Function: draw the desktop
 */
void draw_background() {
    int i;
    int j;
    for (i = 0; i < BG_HEIGHT; i++) {
        for (j = 0; j < BG_WIDTH; j++) {
            *(uint32_t *)(qemu_memory + j + i * BG_WIDTH) = GRERY;
            if (j >= 574) {
                uint32_t rgb = BLACK;
                rgb |= ((((bg[(j-574) + i * 450] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
                rgb |= ((((bg[(j-574) + i * 450] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
                rgb |= (((bg[(j-574) + i * 450] & BLUE_MASK) << 3) & 0xFF);
                *(uint32_t *)(qemu_memory + j + i * BG_WIDTH) = rgb;
            }
        }
    }
}

/*
 * void draw_uiuc()
 * Input: none
 * Return value: none
 * Function: draw the OS text
 */
void draw_os_font() {
    int i, j;
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 100; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((os_font[(j) + i * 100] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((os_font[(j) + i * 100] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((os_font[(j) + i * 100] & BLUE_MASK) << 3) & 0xFF);
            if (rgb == 0x00F80000) {
                rgb = BLACK;
                switch (col)
                {
                case 0xF80000:
                    rgb |= 0xFFA500;
                    break;
                case 0xFFA500:
                    rgb |= 0xFFFF00;
                    break;
                case 0xFFFF00:
                    rgb |= 0x00FF00;
                    break;
                case 0x00FF00:
                    rgb |= 0x007FFF;
                    break;
                case 0x007FFF:
                    rgb |= 0x0000FF;
                    break;   
                case 0x0000FF:
                    rgb |= 0x8B00FF;
                    break;
                case 0x8B00FF:
                    rgb |= 0xF80000;
                    break;
                default:
                    break;
                }
                col = rgb;
            } 
            else {
                rgb = 0x00505050;
            }
            
            *(uint32_t *)(qemu_memory + 2 * j + 2 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 2 * j + (2 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 2 * j + 1 + 2 * i * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + 2 * j + 1 + (2 * i + 1) * BG_WIDTH) = rgb;
        }
    }
}


/*
 * void draw_char(int x, int y, char ch, uint32_t color, uint32_t back_color)
 * Input: x, y  - position in the screen
 *        ch    - the character to draw
 *        color - the color of the character
 *        back_color - the background color
 * Return value: none
 * Function: draw a character
 */
void draw_char(int x, int y, char ch, uint32_t color, uint32_t back_color) {
    char* print_char = (char*)font_data[(uint8_t)(ch)];
    int i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            if (print_char[i] & 1 << (8 - j)) {
                uint32_t rgb = BLACK;
                rgb |= color;
                *(uint32_t *)(qemu_memory + x + j + (i + y) * BG_WIDTH) = rgb;
            }
            else {
                *(uint32_t *)(qemu_memory + x + j + (i + y) * BG_WIDTH) = back_color;
            }
        }
    }
}


/*
 * draw_string(int x, int y, int8_t* str, uint32_t color, uint32_t back_color)
 * Input: x, y  - position in the screen
 *        str    - the string to draw
 *        color - the color of the character
 *        back_color - the background color
 * Return value: none
 * Function: draw a string
 */
void draw_string(int x, int y, int8_t* str, uint32_t color, uint32_t back_color) {
    int i;
    for (i = 0; i < strlen(str); i++)
        draw_char(x + 8 * i, y, str[i], color, back_color);
}


/*
 * void draw_big_char(int x, int y, char ch, uint32_t color, uint32_t back_color)
 * Input: x, y  - position in the screen
 *        ch    - the character to draw
 *        color - the color of the character
 *        back_color - the background color
 * Return value: none
 * Function: draw a big character
 */
void draw_big_char(int x, int y, char ch, uint32_t color, uint32_t back_color) {
    char* print_char = (char*)font_data[(uint8_t)(ch)];
    int i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            if (print_char[i] & 1 << (8 - j)) {
                uint32_t rgb = BLACK;
                rgb |= color;
                *(uint32_t *)(qemu_memory + 2 * x + j + (i + 2 * y) * BG_WIDTH) = rgb;
                *(uint32_t *)(qemu_memory + 2 * x + 1 + j + (i + 2 * y) * BG_WIDTH) = rgb;
                *(uint32_t *)(qemu_memory + 2 * x + j + (i + 2 * y + 1) * BG_WIDTH) = rgb;
                *(uint32_t *)(qemu_memory + 2 * x + 1 + j + (i + 2 * y + 1) * BG_WIDTH) = rgb;
            }
            else {
                *(uint32_t *)(qemu_memory + 2 * x + j + (i + 2 * y) * BG_WIDTH) = back_color;
                *(uint32_t *)(qemu_memory + 2 * x + 1 + j + (i + 2 * y) * BG_WIDTH) = back_color;
                *(uint32_t *)(qemu_memory + 2 * x + j + (i + 2 * y + 1) * BG_WIDTH) = back_color;
                *(uint32_t *)(qemu_memory + 2 * x + j + 1 + (i + 2 * y + 1) * BG_WIDTH) = back_color;
            }
        }
    }
}


/*
 * void boot_gui()
 * Input: none
 * Return value: none
 * Function: boot gui
 */
void boot_gui() {
    draw_transparent();
    draw_boot_font();
    draw_background();
    draw_os_font();
    draw_string(2, BG_HEIGHT - 24 , "Creator: Kunle Li, Ziyuan Lin, Peiyuan Liu", 0x0000FFFF, 0x00505050);
}


/*
 * void draw_temrinal_char(int x, int y, char ch, uint32_t color)
 * Input: x, y  - position in the screen
 *        ch    - the character to draw
 *        color - the color of the character
 * Return value: none
 * Function: draw a character in the terminal
 */
void draw_terminal_char(int x, int y, char ch, uint32_t color) {
    char* print_char = (char*)font_data[(uint8_t)(ch)];
    int i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            if (print_char[i] & 1 << (8 - j)) {
                uint32_t rgb = BLACK;
                rgb |= color;
                *(uint32_t *)(qemu_memory + x + j + (i + y) * BG_WIDTH) = rgb;
            }
            else {
                uint32_t rgb = BLACK;
                rgb |= ((((cloud_term[(x + j - TERMINAL_INIT_X)/2 + (y + i - TERMINAL_INIT_Y)/2 * 324] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
                rgb |= ((((cloud_term[(x + j - TERMINAL_INIT_X)/2+ (y + i - TERMINAL_INIT_Y)/2 * 324] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
                rgb |= (((cloud_term[(x + j - TERMINAL_INIT_X)/2 + (y + i - TERMINAL_INIT_Y)/2 * 324] & BLUE_MASK) << 3) & 0xFF);
                *(uint32_t *)(qemu_memory + x + j + (i + y) * BG_WIDTH) = rgb;
            }
        }
    }
}


/*
 * void draw_temrinal_string(int x, int y, char ch, uint32_t color)
 * Input: x, y  - position in the screen
 *        str   - the string to draw
 *        color - the color of the string
 * Return value: none
 * Function: draw a string in the terminal
 */
void draw_terminal_string(int x, int y, int8_t* str, uint32_t color) {
    int i;
    for (i = 0; i < strlen(str); i++)
        draw_terminal_char(x + 8 * i, y, str[i], color);
}

/*
 * void draw_temrinal_window(int x, int y, int w, int h, uint32_t color)
 * Input: x, y  - position in the screen
 *        w, h  - the width and height of the terminal
 *        color - the color of the terminal
 * Return value: none
 * Function: draw a terminal window
 */
void draw_terminal_window(int x, int y, int w, int h, uint32_t color) {
    int i, j;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((cloud_term[j + i * w] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((cloud_term[j + i * w] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((cloud_term[j + i * w] & BLUE_MASK) << 3) & 0xFF);
            *(uint32_t *)(qemu_memory + x + 2 * j + (y + 2 * i) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + x + 2 * j + 1 + (y + 2 * i) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + x + 2 * j + (y + 2 * i + 1) * BG_WIDTH) = rgb;
            *(uint32_t *)(qemu_memory + x + 2 * j + 1 + (y + 2 * i + 1) * BG_WIDTH) = rgb;
        }
    }
}


/*
 * void draw_temrinal_bar(int x, int y, int w, int h)
 * Input: x, y  - position in the screen
 *        w, h  - the width and height of the terminal
 *        color - the color of the terminal status bar
 * Return value: none
 * Function: draw a terminal status bar
 */
void draw_terminal_bar(int x, int y, int w, int h) {
    int i, j;
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            *(uint32_t *)(qemu_memory + x + i + (y + j) * BG_WIDTH) = WHITE;
        }
    }
}


/*
 * void draw_temrinal(char* terminal_memory, int32_t tid)
 * Input: temrinal_memory - the staring position in the memory to draw
 *        tid             - the process id
 * Return value: none
 * Function: draw a terminal
 */
void draw_terminal(char* terminal_memory, int32_t tid) {
    int i, j;
    if (box) {
        draw_terminal_window(TERMINAL_INIT_X, TERMINAL_INIT_Y, 324, 200, BLACK);
        draw_terminal_bar(TERMINAL_INIT_X, 82, 648, 18);
        if (tid == 0) 
            draw_string(300, 83, "Terminal 1", BLACK, WHITE);
        if (tid == 1) 
            draw_string(300, 83, "Terminal 2", BLACK, WHITE);
        if (tid == 2) 
            draw_string(300, 83, "Terminal 3", BLACK, WHITE);
        box = 0;
    }

    set_video_page(curr_terminal);
    store_vid_mem(curr_terminal);
    if (refresh_terminal) {
        for (i = 0; i < NUM_ROWS; i++) {
            char line[81] = "                                                                                ";
            draw_terminal_string(TERMINAL_INIT_X, TERMINAL_INIT_Y + 16 * i, line, WHITE);
        }
        if (tid == 0) 
            draw_string(300, 83, "Terminal 1", BLACK, WHITE);
        if (tid == 1) 
            draw_string(300, 83, "Terminal 2", BLACK, WHITE);
        if (tid == 2) 
            draw_string(300, 83, "Terminal 3", BLACK, WHITE);
        refresh_terminal = 0;
    }
    for (i = 0; i < NUM_ROWS; i++) {
        char line[81] = "                                                                                ";
        for (j = 0; j < NUM_COLS; j++) {
            if (*(uint8_t *)(terminal_memory + ((j + i * NUM_COLS) << 1)) != '\0')
                line[j] = *(uint8_t *)(terminal_memory + ((j + i * NUM_COLS) << 1));
        }
        draw_terminal_string(TERMINAL_INIT_X, TERMINAL_INIT_Y + 16 * i, line, 0X00FFAA);
    }
}


/*
 * void draw_time_bar()
 * Input: none
 * Return value: none
 * Function: time display
 */
void draw_time_bar() {
    int i;
    char status_bar[26] = "Time:2022-00-00 00:00:00";
    status_bar[10] = month / 10 + ASCII_ZERO;
    status_bar[11] = month % 10 + ASCII_ZERO;
    status_bar[13] = day / 10 + ASCII_ZERO;
    status_bar[14] = day % 10 + ASCII_ZERO;

    if (hour >= 5)
        hour -= 5;
    else
        hour += 19;

    status_bar[16] = hour / 10 + ASCII_ZERO;
    status_bar[17] = hour % 10 + ASCII_ZERO;
    status_bar[19] = mins / 10 + ASCII_ZERO;
    status_bar[20] = mins % 10 + ASCII_ZERO;
    status_bar[22] = sec / 10 + ASCII_ZERO;
    status_bar[23] = sec % 10 + ASCII_ZERO;
    for (i = 0; i < strlen(status_bar); i++)
        draw_big_char(825 + 8 * i, 2, status_bar[i], 0x0000FFFF, 0x00505050);
}


/*
 * void draw_mouse()
 * Input: none
 * Return value: none
 * Function: mouse display
 */
void draw_mouse() {
    int i, j;
    if (mouse_initial == 0) {
        for (i = 0; i < 22; i++) {
            for (j = 0; j < 34; j++) {
                mouse_buffer[i][j] = *(uint32_t *)(qemu_memory + x_pos + i + (j + y_pos)* BG_WIDTH);
            }
        }
        mouse_initial = 1;
    }
    if (last_x == x_pos && last_y == y_pos)
        return;
    for (i = 0; i < 22; i++) {
        for (j = 0; j < 34; j++) {
            *(uint32_t *)(qemu_memory + last_x + i + (j + last_y) * BG_WIDTH) = mouse_buffer[i][j];
        }
    }
    for (i = 0; i < 22; i++) {
        for (j = 0; j < 34; j++) {
            mouse_buffer[i][j] = *(uint32_t *)(qemu_memory + x_pos + i + (j + y_pos)* BG_WIDTH);
        }
    }
    for (i = 0; i < 22; i++) {
        for (j = 0; j < 34; j++) {
            uint32_t rgb = BLACK;
            rgb |= ((((mouse_img[i + j * 22] & RED_MASK) >> 11) << 3) & 0xFF) << 16;
            rgb |= ((((mouse_img[i + j * 22] & GREEN_MASK) >> 5) << 2) & 0xFF) << 8;
            rgb |= (((mouse_img[i + j * 22] & BLUE_MASK) << 3) & 0xFF);
            if (rgb == BLACK)
                continue;
            *(uint32_t *)(qemu_memory + x_pos + i + (j + y_pos)* BG_WIDTH) = rgb;
        }
    }
    last_x = x_pos;
    last_y = y_pos;
}



