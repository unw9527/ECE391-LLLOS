#include "vbe.h"
#include "types.h"
#include "lib.h"

void vbe_write(uint16_t index, uint16_t value) {
    // http://wiki.osdev.org/VGA_Hardware#Memory_Layout_in_16-color_graphics_modes
    outw(index, VBE_DISPI_IOPORT_INDEX);
    outw(value, VBE_DISPI_IOPORT_DATA);
}


void vbe_set(uint16_t xres, uint16_t yres, uint16_t bpp) {
    // http://wiki.osdev.org/VGA_Hardware#Memory_Layout_in_16-color_graphics_modes
    vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    vbe_write(VBE_DISPI_INDEX_XRES, xres);
    vbe_write(VBE_DISPI_INDEX_YRES, yres);
    vbe_write(VBE_DISPI_INDEX_BPP, bpp);
    vbe_write(VBE_DISPI_INDEX_FB_BASE_HI, QEMU_BASE_ADDR >> 16); // write the high 16 bits of QEMU_BASE_ADDR to 0x0B
    vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);
}



