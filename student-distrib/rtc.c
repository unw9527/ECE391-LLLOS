#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "tests.h"
#include "scheduling.h"
#include "terminal.h"

int rtc_active[3]       = {0,0,0};
int rtc_flag[3]         = {1,1,1};
int rtc_counter[3]      = {0,0,0};
int rtc_init_counter[3] = {0,0,0};

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
    int i;
    // if (get_counter() == 3)
    //     test_interrupts();

    outb(REG_C, CMOS_PORT_0);                                             /* Do some strange stuff with register C.*/
    inb(CMOS_PORT_1);
    RTC_intr = 1;
    send_eoi(8); // IRQ 8
    for (i = 0; i < 3; i++)
    {
        rtc_counter[i]--;
        /* Check if counter has reached zero */
        if(rtc_counter[i] == 0)
        {
            /* Clear interrupt flag */
            rtc_flag[i] = 0;
            /* Reset counter */
            rtc_counter[i] = rtc_init_counter[i];
        }
    }
}


/* int32_t RTC_open(const uint8_t * filename);
 * Inputs: filename
 * Return Value: 0
 * Function: provides access to the ﬁle system */
int32_t RTC_open(const uint8_t * filename){
    int32_t freq = 2;

    /* Set RTC to active for the running terminal */
    rtc_active[running_term] = 1;

    rtc_counter[running_term] = 1024/freq;

    rtc_init_counter[running_term] = rtc_counter[running_term];

    return 0;
}


/* int32_t RTC_close(int32_t fd);
 * Inputs: fd
 * Return Value: 0
 * Function: closes the speciﬁed ﬁle descriptor and makes it available for return from later calls to open */
int32_t RTC_close(int32_t fd){
    rtc_active[running_term] = 0;
    return 0;
}


/* int32_t RTC_read(int32_t fd, void * buf, int32_t nbytes);
 * Inputs: fd, buf, nbytes
 * Return Value: 0
 * Function: set a ﬂag and wait until the interrupt handler clears it, then return 0 */
int32_t RTC_read(int32_t fd, void * buf, int32_t nbytes){
    if(rtc_active[running_term] == 1)
        rtc_flag[running_term] = 1;

    
    sti();

    /* Wait until interrupt handler clears flag */
    while (rtc_flag[running_term]);

    cli();
    return 0;
}


/* int32_t RTC_write(int32_t fd, const void * buf, int32_t nbytes);
 * Inputs: fd, buf, nbytes
 * Return Value: 0
 * Function: writes data to the terminal or to a device (RTC) */
int32_t RTC_write(int32_t fd, const void * buf, int32_t nbytes){
    int32_t freq, valid_freq;

    /* Check for null pointer */
    if (buf == NULL)
      return -1;

    /* Check for size */
    if (nbytes != 4)
      return -1;

    /* Get the frequency value */
    freq = *((int32_t*)buf);

    /* Check if frequency is in valid range */
    if (freq < 2 || freq > 8192)
      return -1;

    /* Check if frequency is a power of 2 */
    valid_freq = !(freq & (freq - 1));
    if (valid_freq != 1)
      return -1;

    rtc_counter[running_term] = 1024/freq/4;

    rtc_init_counter[running_term] = rtc_counter[running_term];

    /* Return number of bytes written */
    return 4;
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
