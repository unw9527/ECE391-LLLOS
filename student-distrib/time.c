#include "time.h"
#include "rtc.h"
#include "lib.h"
#include "terminal.h"
#include "gui.h"
// Reference: https://wiki.osdev.org/CMOS#Getting_Current_Date_and_Time_from_RTC


int get_update_in_progress_flag() {
    outb(0x0A, CMOS_ADDR);
    int input = inb(CMOS_DATA);
    return input & 0x80;
}

unsigned get_RTC_register(int reg) {
    outb(reg, CMOS_ADDR);
    return inb(CMOS_DATA);
}

void get_system_time() {
    int cnt = 0;
    while (get_update_in_progress_flag() || cnt>=10000) ++cnt;
    sec = get_RTC_register(0x00);
    mins = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);

    int Mode = get_RTC_register(0x0B);
    if (!(Mode & 0x04)) {
        hour = (hour >> 4) * 10 + (hour & 0x0f);
        mins = (mins >> 4) * 10 + (mins & 0x0f);
        sec = (sec >> 4) * 10 + (sec & 0x0f);
        year = (year >> 4) * 10 + (year & 0x0f);
        month = (month >> 4) * 10 + (month & 0x0f);
        day = (day >> 4) * 10 + (day & 0x0f);
    }

    if (!(Mode & 0x02) && (hour & 0x80)) hour = ((hour & 0x7F) + 12) % 24;
    draw_time_bar();
}

