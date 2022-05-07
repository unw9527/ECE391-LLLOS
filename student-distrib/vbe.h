#ifndef _VBE_H
#define _VBE_H

#include "types.h"

// code is mainly from https://forum.osdev.org/viewtopic.php?f=1&t=30884

#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF
#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5


#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6

#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_GETCAPS               0x02
#define VBE_DISPI_8BIT_DAC              0x20
#define VBE_DISPI_LFB_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

#define QEMU_BASE_ADDR                  0xFD000000
#define VBE_DISPI_INDEX_FB_BASE_HI      0x0B

#define BG_WIDTH                        1024
#define BG_HEIGHT                       768


/* Contains the high 16 bits of the linear framebuffer address (low 16 bits are zero). (from https://wiki.osdev.org/User:Kazinsal/VirtualBox_Video_Adapter) */

extern void vbe_set(uint16_t xres, uint16_t yres, uint16_t bpp);

#endif


