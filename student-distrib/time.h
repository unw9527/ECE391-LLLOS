#ifndef _TIME_H
#define _TIME_H

#include "types.h"

#define CMOS_ADDR   0x70
#define CMOS_DATA   0x71

int32_t sec, mins, hour, day, month, year;

extern void get_system_time();

#endif //MP3_GROUP_14_CMOS_H


