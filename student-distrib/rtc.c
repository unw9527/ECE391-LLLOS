#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "tests.h"

/* void RTC_init(void);
 * Inputs: void
 * Return Value: none
 * Function: initialize the RTC */
void RTC_init()
{
    uint32_t flags;
    unsigned char prev;
    unsigned char rate = 3; 
    RTC_intr = 0;                                         /* The maximux rates is 32784 >> 3 = 4096HZ*/
    cli_and_save(flags);
    outb(REG_B, CMOS_PORT_0);
    prev = inb(CMOS_PORT_1);
    outb(REG_B, CMOS_PORT_0);
    outb(prev | 0x40, CMOS_PORT_1);                                       /* To initialize the RTC with the sixth bit set.*/
    outb(REG_A, CMOS_PORT_0);
    prev = inb(CMOS_PORT_1);
    outb(REG_A, CMOS_PORT_0);
    outb((prev & 0xF0) | rate, CMOS_PORT_1);                              /* Mask the value with 0xF0 and set the rates to be 3.*/
    outb(inb(CMOS_PORT_0) & 0x7F, CMOS_PORT_0);                                /* Enable NMI*/
    inb(CMOS_PORT_1);
    outb(REG_C, CMOS_PORT_0);                                             /* Do some strange stuff with register C.*/
    inb(CMOS_PORT_1);
    restore_flags(flags);
    sti();
    enable_irq(8);                                                   /* IRQ8 corresponds to rtc.*/
}


/* void RTC_handler(void);
 * Inputs: void
 * Return Value: none
 * Function: handle the RTC */
void RTC_handler()
{
    // if (get_counter() == 3)
    //     test_interrupts();
    send_eoi(8); // IRQ 8
    outb(REG_C, CMOS_PORT_0);                                             /* Do some strange stuff with register C.*/
    inb(CMOS_PORT_1);
    RTC_intr = 1;
}


/* int32_t RTC_open(const uint8_t * filename);
 * Inputs: filename
 * Return Value: 0
 * Function: provides access to the ﬁle system */
int32_t RTC_open(const uint8_t * filename){
    RTC_set_freq(RTC_RATE_2); // set to 2 Hz
    return 0;
}


/* int32_t RTC_close(int32_t fd);
 * Inputs: fd
 * Return Value: 0
 * Function: closes the speciﬁed ﬁle descriptor and makes it available for return from later calls to open */
int32_t RTC_close(int32_t fd){
    return 0;
}


/* int32_t RTC_read(int32_t fd, void * buf, int32_t nbytes);
 * Inputs: fd, buf, nbytes
 * Return Value: 0
 * Function: set a ﬂag and wait until the interrupt handler clears it, then return 0 */
int32_t RTC_read(int32_t fd, void * buf, int32_t nbytes){
    RTC_intr = 0;
    while (0 == RTC_intr){} // wait here
    RTC_intr = 0; // reset
    return 0;
}


/* int32_t RTC_write(int32_t fd, const void * buf, int32_t nbytes);
 * Inputs: fd, buf, nbytes
 * Return Value: 0
 * Function: writes data to the terminal or to a device (RTC) */
int32_t RTC_write(int32_t fd, const void * buf, int32_t nbytes){
    int power;
    char rate;
    if (NULL == buf || 4 != nbytes) return -1;
    int32_t freq = *((int32_t*) buf);
    if ((freq & (freq - 1)) != 0) return -1; // one line formula to check whether freq is a power of 2
    power = 0;
    while (1 < freq){
        freq >>= 1;
        power++;
    }
    // the following values are found in the datasheet of RTC 
    rate = 16 - power;
    // mp3 doc further requires frequency not going beyond 1024 Hz, not below 2 Hz
    if (RTC_RATE_1024 > rate || RTC_RATE_2 < rate) return -1;
    RTC_set_freq(rate);
    return 0;
}


/* void RTC_set_freq(int32_t rate);
 * Inputs: freq
 * Return Value: None
 * Function: change the rate based on the frequency in Hz */
void RTC_set_freq(int32_t rate){
    char prev;
    cli(); // disable interrupts
    outb(REG_A, CMOS_PORT_0); // select register A, and disable NMI
    prev = inb(CMOS_PORT_1); // read the current value of register A
    outb(REG_A, CMOS_PORT_0); // set the index again
    outb((prev & 0xF0) | rate, CMOS_PORT_1);
    sti();

}

int32_t (*read_rtc_pt) (int32_t, void*, int32_t) = RTC_read;
int32_t (*open_rtc_pt) (const uint8_t*) = RTC_open;
int32_t (*close_rtc_pt) (int32_t) = RTC_close;
int32_t (*write_rtc_pt) (int32_t, const void*, int32_t) = RTC_write;
