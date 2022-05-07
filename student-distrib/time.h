#ifndef _TIME_H
#define _TIME_H

// Reference: https://wiki.osdev.org/CMOS#Getting_Current_Date_and_Time_from_RTC
#include "types.h"

#define CMOS_ADDR   0x70
#define CMOS_DATA   0x71

int32_t sec, mins, hour, day, month, year;

extern void get_system_time();

#endif


