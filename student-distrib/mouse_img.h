#ifndef _MOUSE_IMG_H
#define _MOUSE_IMG_H

#include "types.h"
#include "vbe.h"

// Resolution: 34 * 22

const int16_t mouse_img[34 * 22] = {
0x841,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8c51,0x39c7,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x8c30,0xdedb,0x4208,0x841,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x736e,0xd67a,0xe73c,0x39e7,0x861,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x632c,0xb596,0xf79e,0xe71c,0x4a49,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6b4d,0x9492,0xdedb,0xffdf,0xef3c,0x4228,0x841,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x632d,0x8c71,0xbdd7,0xf7be,0xffdf,0xef3c,0x5289,0x20,0x20,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x634d,0x9492,0x94b2,0xf79e,0xf79e,0xffdf,0xef7d,0x528a,0x20,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x634d,0x8c71,0x9492,0xbdd7,0xffdf,0xf79e,0xffde,0xef5d,0x62eb,0x841,0x841,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6b6d,0x8c91,0x8c92,0x9cf3,0xef5d,0xf7be,0xffbe,0xf79e,0xef7d,0x5aeb,0x861,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6b8e,0x94b2,0x8c92,0x8c92,0xc659,0xffdf,0xf7bf,0xf79e,0xffff,0xef5d,0x630c,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x6b6d,0x8cd2,0x8cb2,0x8cb2,0xa555,0xe77d,0xef9e,0xf7df,0xef7e,0xffbf,0xef7d,0x6b6e,0x841,0x0,0x0,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x6b8e,0x94d3,0x8cd3,0x94f3,0x8cd3,0xcebb,0xefbf,0xef9e,0xefbf,0xf7df,0xef9e,0xf7bf,0x738e,0x841,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x73af,0x94d3,0x9514,0x84b3,0xa5b6,0xadf8,0xc6db,0xe7df,0xefbf,0xef7e,0xef7e,0xf7bf,0xf79e,0x8410,0x40,0x20,0x0,0x0,0x0,0x0,0x0,0x0,0x736e,0x9cf4,0x94f4,0x8cf4,0x9db6,0x84f4,0x536d,0xb67a,0xe79f,0xe77e,0xef9f,0xe75d,0xf7df,0xef7e,0x8c71,0x61,0x0,0x0,0x0,0x0,0x0,0x0,0x73af,0x9cf4,0x9d35,0x8d14,0xa5d7,0x8d35,0x6410,0xa5f8,0xb639,0xe79f,0xe75e,0xe73d,0xef7e,0xef7e,0xf7be,0x8c92,0x20,0x0,0x0,0x0,0x0,0x0,0x7bb0,0x9d14,0x9553,0x8d53,0x9df8,0x7536,0x4c32,0xa6fd,0x9ebb,0x9e18,0xdf9e,0xe77e,0xdf3d,0xdf7d,0xef9e,0xf7bf,0x94b2,0x861,0x0,0x0,0x0,0x0,0x73b0,0x9d34,0x9d53,0x8d33,0xa619,0x74f6,0x3bb1,0x8619,0xb79e,0xa67a,0x9596,0xdf9e,0xdf7e,0xcefb,0xdf3c,0xe77d,0xf7be,0xa534,0x841,0x0,0x0,0x0,0x73d0,0x9d34,0x9d54,0x8d54,0xa65a,0x6cf6,0x3391,0x6556,0xaf7e,0xc7bf,0xae59,0x9576,0xd77d,0xd73c,0xdf5c,0xd6fb,0xf7be,0xf7de,0xad75,0x10a2,0x0,0x0,0x73f0,0x9d35,0x9d54,0x9535,0xa65a,0x6cf6,0x33b2,0x4cd6,0x96dd,0xc7ff,0xc79e,0xa67a,0x8575,0xcf7d,0xd77d,0xcefb,0xe75d,0xe75d,0xf7de,0xad75,0x18e3,0x20,0x6bcf,0x9d55,0x9514,0x8cf4,0xa63b,0x64d6,0x2392,0x44b6,0x65b9,0xb7df,0xbfdf,0xbfbf,0x967a,0x7514,0xc77d,0xd79d,0xd6db,0xdefb,0xdf1c,0xf7be,0xbdf8,0x18e4,0x6bae,0x9513,0x8cd3,0x8cd5,0xa63b,0x64d6,0x23b3,0x3cb7,0x3cd7,0x8f3f,0xafff,0xa7bf,0xafbf,0x865a,0x7535,0xcf7d,0xe75d,0xef7d,0xdf1c,0xef7e,0xf79e,0x6b4d,0x63ad,0x8492,0x8c93,0x84b4,0xa61b,0x64b6,0x2bd4,0x3cb8,0x2c77,0x55bb,0x8f7f,0x873f,0x7efe,0x763b,0x6d56,0xbf3d,0xdf1c,0xd6ba,0xce9a,0xef7d,0xce59,0x1082,0x5b6c,0x8451,0x7c31,0x7c32,0xae1a,0x6475,0x2bb3,0x3c98,0x13b5,0x1bf5,0x55fc,0x5e5d,0x4dfb,0x5db9,0x7dd9,0xc77f,0xce9a,0xce9a,0xd6fb,0xe73c,0x4a6a,0x0,0x5b2b,0x7c30,0x73f0,0x7c11,0xa5d9,0x6454,0x2b31,0x1b53,0x2b1,0x2f2,0x3d1a,0x565d,0x4ddb,0x44f7,0xa71e,0xae7c,0xc659,0xc659,0xdf1c,0x9d14,0x841,0x0,0x4aca,0x6bcf,0x6b6e,0x6b8f,0xadd9,0x7cd5,0x22ae,0xa4e,0x270,0x290,0x1bd4,0x561d,0x3cf7,0x65ba,0xa6bd,0xae1a,0xbe18,0xbe18,0xce9a,0x2145,0x0,0x0,0x4a8a,0x634d,0x5b2d,0x634e,0xadb7,0xae3a,0x8537,0x5c55,0x3352,0xa6f,0xaaf,0x44d7,0x44b6,0x9edd,0x9df9,0xadb8,0xbdf7,0xc659,0x6b6d,0x0,0x0,0x0,0x3a48,0x5b0c,0x5b0c,0x73cf,0x73f0,0x7c32,0x84d5,0xa61a,0x9e1b,0x9e1c,0x64d7,0x5495,0x967c,0x9639,0x9d76,0xad76,0xb5d6,0xa555,0x20,0x0,0x0,0x0,0x4249,0x5b0c,0x5b2c,0x634d,0x4a6a,0x5b0d,0x5b4e,0x5b6e,0x6bf1,0x84b5,0xa5fa,0xb6bd,0xb6dc,0x84f4,0xa575,0xa514,0xad96,0x3a08,0x0,0x0,0x0,0x0,0x3a07,0x5aeb,0x52aa,0x4a49,0x4a8a,0x4a8a,0x52eb,0x532c,0x5b2d,0x4a8b,0x5b0e,0x5b2e,0x7c72,0x9d75,0x94f3,0xad95,0x94b2,0x0,0x20,0x0,0x0,0x0,0x10a2,0x31c6,0x31c6,0x4208,0x4229,0x4a49,0x4a8a,0x4aa9,0x52ca,0x528a,0x526b,0x526b,0x52ec,0xa575,0x94f3,0xadb6,0x2124,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x20,0x862,0x18c3,0x2945,0x31c6,0x4268,0x4a88,0x528a,0x524a,0x524a,0x4229,0x8cb1,0xa595,0x73ef,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x41,0x10a2,0x1904,0x31c6,0x4208,0x4228,0x4228,0x632c,0xad75,0x1082,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x861,0x861,0x841,0x20,0x20,0x1082,0x10c3,0x3186,0x31a6,0x4228,0x0,0x0,0x0,0x0,0x0,0x0,0x0};

#endif
