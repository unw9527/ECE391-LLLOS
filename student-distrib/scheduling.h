#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "i8259.h"
#include "types.h"
#include "process.h"

#define DIVISOR (int)(1193180 / 100) // 100 as recommended online
#define COMMAND_REG 0x43
#define RES_BYTE    0x36
#define CHANNEL_0_DP    0x40

// initialize PIT
extern void PIT_init();
// swap process to execute
extern void PIT_handler();

#endif

