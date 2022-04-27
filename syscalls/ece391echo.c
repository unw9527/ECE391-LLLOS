#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t cnt;
    uint8_t buf[1024];

    if (0 != ece391_getargs (buf, 1024))
        return 0;
    cnt = ece391_read(1, buf, 1024);
    buf[cnt] = '\0';
    ece391_fdputs (1, buf);
    ece391_fdputs (1, (uint8_t*)"\n");
    return 0;
}

