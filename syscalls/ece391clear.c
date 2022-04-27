#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t i;
    int32_t cnt;
    uint8_t buf[80*25];
    for (i = 0; i < 80*25-1; i++)
        buf[i] = ' ';
    buf[80*25-1] = '\0';
    ece391_fdputs (1, buf);
    for (i = 0; i < 80*25-1; i++)
        buf[i] = 0x08;
    buf[80*25-1] = '\0';
    ece391_fdputs (1, buf);
    return 0;
}
