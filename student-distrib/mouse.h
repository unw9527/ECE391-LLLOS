#ifndef _MOUSE_H
#define _MOUSE_H

#include "lib.h"

// Reference: https://wiki.osdev.org/Mouse
#define COMPAQ 0x20
#define MOUSE_AUX 0x64
#define MOUSE_DATA 0x60
#define CLEAR_BIT5 0xDF
#define COMMAND_REQUIRE 0xD4
#define SET_SAMPLE 0xF3
#define ACK 0xFA
#define ENABLE_PACKET 0xF4
#define MOUSE_RESET 0xFF
#define GREEN 0x2
#define X_SIGN 0x10
#define Y_SIGN 0x20
#define X_OVERFLOW 0x40
#define Y_OVERFLOW 0x80
#define RESOLUTION 0xE8
#define MIDDLE_BUTTON 0x04
#define RIGHT_BUTTON 0x02
#define LEFT_BUTTON 0x01

extern int32_t x_pos;
extern int32_t y_pos;

void mouse_handler();
void mouse_init();
void text_mode_mouse();
void mouse_update_vid(uint8_t status, uint8_t x, uint8_t y);
void mouse_click(uint8_t status);

#endif
