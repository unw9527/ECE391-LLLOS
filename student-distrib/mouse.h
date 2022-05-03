#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"

#define COMPAQ 0x20
#define MOUSE_AUX 0x64
#define MOUSE_DATA 0x60
#define CLEAR_BIT5 0xDF
#define COMMAND_REQUIRE 0xD4
#define SET_SAMPLE 0xF3
#define ACK 0xFA
#define ENABLE_PACKET 0xF4
#define MOUSE_RESET 0xFF

void mouse_handler();
void mouse_init();
void text_mode_mouse();
void mouse_update_vid(uint8_t x, uint8_t y);

#endif
