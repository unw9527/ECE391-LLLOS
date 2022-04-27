#ifndef _RTC_H
#define _RTC_H
#define CMOS_PORT_0 0x70
#define CMOS_PORT_1 0x71
#define REG_A       0x8A
#define REG_B       0x8B
#define REG_C       0x0C

// mp3.2
#define RTC_RATE_2      0xf // 2 Hz
#define RTC_RATE_1024   0x6 // 1024 Hz

// mp3.5
#define MAX_FREQ    8192
#define MIN_FREQ    2
#define FREQ_COEF   8 // based on trial

volatile int32_t RTC_intr;
// initialize RTC
extern void RTC_init();
// interrupt handler of RTC
extern void RTC_handler();
extern int32_t RTC_open(const uint8_t * filename);
extern int32_t RTC_read(int32_t fd, void * buf, int32_t nbytes);
extern int32_t RTC_write(int32_t fd, const void * buf, int32_t nbytes);
extern int32_t RTC_close(int32_t fd);
extern void RTC_set_freq(int32_t rate);

extern int rtc_active[3];
extern int rtc_flag[3];
extern int rtc_counter[3];

#endif

