#ifndef _RTC_H
#define _RTC_H
#define PORT_0 0x70
#define PORT_1 0x71
#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x0C

extern void RTC_init();
extern void RTC_handler();

#endif
