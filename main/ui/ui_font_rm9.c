/*******************************************************************************
 * Size: 9 px
 * Bpp: 4
 * Opts: --bpp 4 --size 9 --no-compress --font ../ASSETS/Roboto-Medium.ttf --range 0x20-0x7f,0xb0,0xb7,0x2013,0x2014,0x2026 --format lvgl
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl.h"
#endif

#ifndef UI_FONT_RM9
#define UI_FONT_RM9 1
#endif

#if UI_FONT_RM9

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x5c, 0x4b, 0x4b, 0x4b, 0x27, 0x1, 0x4b,

    /* U+0022 "\"" */
    0x96, 0x89, 0x57, 0x31, 0x20,

    /* U+0023 "#" */
    0x0, 0xb1, 0xa0, 0x0, 0xb3, 0x70, 0x2c, 0xdd,
    0xc3, 0x5, 0x69, 0x20, 0x6d, 0xce, 0xb0, 0xa,
    0x1b, 0x0, 0xb, 0x1a, 0x0,

    /* U+0024 "$" */
    0x0, 0x90, 0x0, 0x8f, 0xb1, 0x3e, 0x1a, 0x83,
    0xe1, 0x22, 0x9, 0xf9, 0x0, 0x2, 0xc7, 0x79,
    0x7, 0x91, 0xdd, 0xe3, 0x0, 0xb0, 0x0,

    /* U+0025 "%" */
    0x4c, 0xb0, 0x10, 0x8, 0x3a, 0x2a, 0x0, 0x2b,
    0x97, 0x40, 0x0, 0x1, 0xa0, 0x0, 0x0, 0x94,
    0xa9, 0x0, 0x28, 0x75, 0x93, 0x2, 0x13, 0xcb,
    0x0,

    /* U+0026 "&" */
    0x8, 0xea, 0x0, 0xf, 0xd, 0x20, 0xe, 0x7d,
    0x0, 0xa, 0xf2, 0x10, 0x6c, 0x9a, 0xb3, 0x88,
    0xc, 0xe0, 0x1c, 0xcc, 0xd4,

    /* U+0027 "'" */
    0xa2, 0xa1, 0x30,

    /* U+0028 "(" */
    0x0, 0x20, 0x3a, 0xc, 0x13, 0xb0, 0x69, 0x6,
    0x80, 0x59, 0x1, 0xc0, 0xa, 0x30, 0x1a, 0x0,
    0x0,

    /* U+0029 ")" */
    0x20, 0x8, 0x50, 0xd, 0x0, 0x95, 0x7, 0x80,
    0x69, 0x7, 0x70, 0xa4, 0x1c, 0x9, 0x20, 0x0,
    0x0,

    /* U+002A "*" */
    0x5, 0x60, 0x9b, 0xb9, 0xc, 0xc0, 0x36, 0x72,

    /* U+002B "+" */
    0x0, 0xd0, 0x0, 0xf, 0x0, 0xae, 0xfe, 0x91,
    0x1f, 0x11, 0x0, 0xf0, 0x0,

    /* U+002C "," */
    0x68, 0x95, 0x40,

    /* U+002D "-" */
    0x9e, 0x80,

    /* U+002E "." */
    0x1, 0x4b,

    /* U+002F "/" */
    0x0, 0xa2, 0x0, 0xc0, 0x5, 0x70, 0xb, 0x10,
    0x1b, 0x0, 0x76, 0x0, 0xc0, 0x0,

    /* U+0030 "0" */
    0xa, 0xeb, 0x5, 0xc0, 0xa7, 0x78, 0x7, 0x98,
    0x80, 0x6a, 0x78, 0x7, 0x95, 0xc0, 0xa6, 0xa,
    0xeb, 0x0,

    /* U+0031 "1" */
    0x4, 0xb5, 0x3b, 0xd6, 0x0, 0xb6, 0x0, 0xb6,
    0x0, 0xb6, 0x0, 0xb6, 0x0, 0xb6,

    /* U+0032 "2" */
    0xb, 0xec, 0x18, 0x90, 0xa7, 0x11, 0xa, 0x60,
    0x4, 0xe0, 0x2, 0xe3, 0x0, 0xd5, 0x0, 0x7f,
    0xee, 0xa0,

    /* U+0033 "3" */
    0x1b, 0xeb, 0x7, 0x90, 0xb6, 0x0, 0xb, 0x50,
    0x3e, 0xd0, 0x0, 0xa, 0x77, 0x80, 0x98, 0x1c,
    0xeb, 0x10,

    /* U+0034 "4" */
    0x0, 0x4f, 0x10, 0xc, 0xf1, 0x5, 0xaf, 0x10,
    0xd2, 0xf1, 0x5a, 0xf, 0x1b, 0xee, 0xfc, 0x0,
    0xf, 0x10,

    /* U+0035 "5" */
    0xe, 0xff, 0x80, 0xf0, 0x0, 0x1e, 0x0, 0x3,
    0xfd, 0xc2, 0x2, 0x8, 0xa3, 0x80, 0x7a, 0xa,
    0xed, 0x20,

    /* U+0036 "6" */
    0x3, 0xcb, 0x0, 0xd4, 0x0, 0x4d, 0xdc, 0x17,
    0xc0, 0x98, 0x79, 0x5, 0xb3, 0xd0, 0x88, 0x8,
    0xec, 0x10,

    /* U+0037 "7" */
    0xae, 0xee, 0xb0, 0x0, 0xa6, 0x0, 0x1e, 0x0,
    0x7, 0x90, 0x0, 0xe2, 0x0, 0x5c, 0x0, 0xb,
    0x60, 0x0,

    /* U+0038 "8" */
    0xa, 0xeb, 0x4, 0xd0, 0xb6, 0x3d, 0xb, 0x50,
    0xcf, 0xd0, 0x6b, 0x9, 0x87, 0xa0, 0x89, 0x1b,
    0xec, 0x10,

    /* U+0039 "9" */
    0xb, 0xea, 0x6, 0xb0, 0xc5, 0x87, 0x8, 0x86,
    0xb0, 0xb8, 0xb, 0xdd, 0x60, 0x2, 0xd1, 0x9,
    0xc4, 0x0,

    /* U+003A ":" */
    0x4a, 0x1, 0x0, 0x1, 0x4a,

    /* U+003B ";" */
    0x69, 0x1, 0x0, 0x12, 0x59, 0x94, 0x10,

    /* U+003C "<" */
    0x0, 0x5b, 0x5d, 0xb4, 0x8d, 0x60, 0x2, 0x9e,
    0x0, 0x1,

    /* U+003D "=" */
    0x5e, 0xee, 0x60, 0x0, 0x0, 0x5e, 0xee, 0x60,

    /* U+003E ">" */
    0x69, 0x20, 0x1, 0x6c, 0xb2, 0x3, 0x9e, 0x36,
    0xd6, 0x0, 0x10, 0x0, 0x0,

    /* U+003F "?" */
    0x3d, 0xe7, 0x8, 0x62, 0xf0, 0x0, 0x4d, 0x0,
    0x1e, 0x40, 0x6, 0x90, 0x0, 0x0, 0x0, 0x6,
    0x80, 0x0,

    /* U+0040 "@" */
    0x0, 0x7b, 0xba, 0x20, 0x8, 0x60, 0x1, 0xb1,
    0x1a, 0x8, 0xb5, 0x46, 0x65, 0x49, 0x39, 0x19,
    0x83, 0x84, 0x47, 0x9, 0x74, 0x93, 0x77, 0x47,
    0x57, 0x4c, 0x8c, 0xa0, 0xc, 0x20, 0x0, 0x0,
    0x1, 0xab, 0xb5, 0x0,

    /* U+0041 "A" */
    0x0, 0xaa, 0x0, 0x0, 0xee, 0x0, 0x5, 0x99,
    0x50, 0xb, 0x34, 0xb0, 0x1f, 0xff, 0xf1, 0x69,
    0x0, 0xa6, 0xc5, 0x0, 0x6b,

    /* U+0042 "B" */
    0x5f, 0xee, 0x60, 0x5c, 0x3, 0xf0, 0x5c, 0x3,
    0xe0, 0x5f, 0xdf, 0x80, 0x5c, 0x1, 0xf1, 0x5c,
    0x1, 0xf2, 0x5f, 0xee, 0x80,

    /* U+0043 "C" */
    0x6, 0xee, 0x80, 0x3e, 0x20, 0xd4, 0x7a, 0x0,
    0x32, 0x89, 0x0, 0x0, 0x7a, 0x0, 0x22, 0x3e,
    0x10, 0xd4, 0x6, 0xee, 0x80,

    /* U+0044 "D" */
    0x5f, 0xfc, 0x30, 0x5c, 0x4, 0xe0, 0x5c, 0x0,
    0xc5, 0x5c, 0x0, 0xb6, 0x5c, 0x0, 0xc5, 0x5c,
    0x4, 0xe0, 0x5f, 0xec, 0x30,

    /* U+0045 "E" */
    0x5f, 0xff, 0xb5, 0xc0, 0x0, 0x5c, 0x0, 0x5,
    0xfe, 0xe5, 0x5c, 0x0, 0x5, 0xc0, 0x0, 0x5f,
    0xee, 0xc0,

    /* U+0046 "F" */
    0x5f, 0xff, 0xa5, 0xc0, 0x0, 0x5c, 0x0, 0x5,
    0xfe, 0xe4, 0x5c, 0x0, 0x5, 0xc0, 0x0, 0x5c,
    0x0, 0x0,

    /* U+0047 "G" */
    0x6, 0xee, 0x90, 0x2e, 0x20, 0xc6, 0x7a, 0x0,
    0x0, 0x89, 0xc, 0xe8, 0x7a, 0x0, 0x89, 0x2f,
    0x20, 0xa9, 0x6, 0xee, 0xb2,

    /* U+0048 "H" */
    0x5c, 0x0, 0x6b, 0x5c, 0x0, 0x6b, 0x5c, 0x0,
    0x6b, 0x5f, 0xee, 0xfb, 0x5c, 0x0, 0x6b, 0x5c,
    0x0, 0x6b, 0x5c, 0x0, 0x6b,

    /* U+0049 "I" */
    0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d,

    /* U+004A "J" */
    0x0, 0xb, 0x60, 0x0, 0xb6, 0x0, 0xb, 0x60,
    0x0, 0xb6, 0x0, 0xb, 0x5b, 0x70, 0xe4, 0x3d,
    0xe9, 0x0,

    /* U+004B "K" */
    0x5c, 0x2, 0xf3, 0x5c, 0xd, 0x70, 0x5c, 0xaa,
    0x0, 0x5f, 0xf7, 0x0, 0x5e, 0x5f, 0x20, 0x5c,
    0x8, 0xc0, 0x5c, 0x0, 0xd6,

    /* U+004C "L" */
    0x5c, 0x0, 0x5, 0xc0, 0x0, 0x5c, 0x0, 0x5,
    0xc0, 0x0, 0x5c, 0x0, 0x5, 0xc0, 0x0, 0x5f,
    0xee, 0x90,

    /* U+004D "M" */
    0x5f, 0x40, 0x6, 0xf3, 0x5f, 0x90, 0xb, 0xf3,
    0x5b, 0xe0, 0x1e, 0xd3, 0x5b, 0xb4, 0x68, 0xd3,
    0x5c, 0x5a, 0xc3, 0xe3, 0x5c, 0xe, 0xd0, 0xe3,
    0x5c, 0xa, 0x80, 0xe3,

    /* U+004E "N" */
    0x5f, 0x10, 0x6b, 0x5f, 0x90, 0x6b, 0x5d, 0xe3,
    0x6b, 0x5c, 0x6c, 0x6b, 0x5c, 0xd, 0xbb, 0x5c,
    0x4, 0xfb, 0x5c, 0x0, 0xab,

    /* U+004F "O" */
    0x5, 0xee, 0x80, 0x2e, 0x21, 0xd5, 0x7a, 0x0,
    0x7a, 0x89, 0x0, 0x5b, 0x7a, 0x0, 0x7a, 0x2e,
    0x21, 0xd5, 0x6, 0xee, 0x80,

    /* U+0050 "P" */
    0x5f, 0xfe, 0x70, 0x5c, 0x1, 0xe3, 0x5c, 0x0,
    0xb6, 0x5c, 0x1, 0xe3, 0x5f, 0xee, 0x70, 0x5c,
    0x0, 0x0, 0x5c, 0x0, 0x0,

    /* U+0051 "Q" */
    0x6, 0xee, 0x80, 0x2e, 0x21, 0xd5, 0x7a, 0x0,
    0x7a, 0x98, 0x0, 0x6b, 0x7a, 0x0, 0x7a, 0x3e,
    0x21, 0xd5, 0x6, 0xef, 0xe0, 0x0, 0x0, 0xa7,
    0x0, 0x0, 0x0,

    /* U+0052 "R" */
    0x5f, 0xee, 0x70, 0x5c, 0x1, 0xf2, 0x5c, 0x2,
    0xf1, 0x5f, 0xef, 0x50, 0x5c, 0xc, 0x60, 0x5c,
    0x5, 0xd0, 0x5c, 0x0, 0xe4,

    /* U+0053 "S" */
    0xa, 0xed, 0x40, 0x6c, 0x3, 0xf0, 0x4e, 0x30,
    0x10, 0x7, 0xed, 0x30, 0x11, 0x6, 0xe0, 0x8a,
    0x2, 0xf0, 0xa, 0xee, 0x60,

    /* U+0054 "T" */
    0xbf, 0xff, 0xf4, 0x0, 0xd4, 0x0, 0x0, 0xd4,
    0x0, 0x0, 0xd4, 0x0, 0x0, 0xd4, 0x0, 0x0,
    0xd4, 0x0, 0x0, 0xd4, 0x0,

    /* U+0055 "U" */
    0x7a, 0x0, 0xc5, 0x7a, 0x0, 0xc5, 0x7a, 0x0,
    0xc5, 0x7a, 0x0, 0xc5, 0x7a, 0x0, 0xc5, 0x4d,
    0x1, 0xe2, 0x8, 0xee, 0x60,

    /* U+0056 "V" */
    0xc7, 0x0, 0xa9, 0x6c, 0x0, 0xe4, 0x1f, 0x14,
    0xe0, 0xc, 0x69, 0x90, 0x7, 0xbe, 0x40, 0x1,
    0xfe, 0x0, 0x0, 0xc9, 0x0,

    /* U+0057 "W" */
    0xa6, 0x9, 0x90, 0x7a, 0x79, 0xd, 0xd0, 0xa6,
    0x4c, 0xd, 0xd1, 0xd3, 0xf, 0x49, 0x95, 0xf0,
    0xd, 0xa5, 0x6c, 0xc0, 0xa, 0xf1, 0x2f, 0x90,
    0x6, 0xd0, 0xe, 0x60,

    /* U+0058 "X" */
    0x8c, 0x1, 0xf3, 0xe, 0x5a, 0xa0, 0x6, 0xef,
    0x10, 0x0, 0xfa, 0x0, 0x6, 0xdf, 0x20, 0xe,
    0x49, 0xb0, 0x8b, 0x1, 0xf4,

    /* U+0059 "Y" */
    0xb7, 0x1, 0xf3, 0x3e, 0x8, 0xb0, 0xc, 0x7e,
    0x30, 0x4, 0xfb, 0x0, 0x0, 0xd5, 0x0, 0x0,
    0xd4, 0x0, 0x0, 0xd4, 0x0,

    /* U+005A "Z" */
    0x9f, 0xff, 0xf0, 0x0, 0xb, 0x80, 0x0, 0x5e,
    0x0, 0x0, 0xe4, 0x0, 0x8, 0xa0, 0x0, 0x3e,
    0x10, 0x0, 0x9f, 0xee, 0xe1,

    /* U+005B "[" */
    0x6e, 0x56, 0xa0, 0x6a, 0x6, 0xa0, 0x6a, 0x6,
    0xa0, 0x6a, 0x6, 0xa0, 0x6e, 0x50,

    /* U+005C "\\" */
    0xb5, 0x0, 0x5b, 0x0, 0xe, 0x10, 0x9, 0x70,
    0x3, 0xd0, 0x0, 0xc3, 0x0, 0x69,

    /* U+005D "]" */
    0xdc, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d,
    0xdc,

    /* U+005E "^" */
    0x4, 0x30, 0xd, 0xa0, 0x3b, 0xd1, 0x95, 0x76,

    /* U+005F "_" */
    0xdd, 0xdd, 0x0,

    /* U+0060 "`" */
    0x44, 0x1, 0xd1,

    /* U+0061 "a" */
    0x1b, 0xda, 0x3, 0x60, 0xd3, 0x1a, 0xbe, 0x48,
    0x90, 0xd4, 0x3e, 0xdd, 0x50,

    /* U+0062 "b" */
    0x79, 0x0, 0x7, 0x90, 0x0, 0x7d, 0xed, 0x17,
    0xb0, 0x99, 0x79, 0x5, 0xb7, 0xb0, 0x99, 0x7c,
    0xed, 0x10,

    /* U+0063 "c" */
    0x1b, 0xeb, 0x7, 0xa0, 0x94, 0xa7, 0x0, 0x7,
    0xa0, 0x83, 0xb, 0xea, 0x0,

    /* U+0064 "d" */
    0x0, 0x8, 0x80, 0x0, 0x88, 0x1c, 0xed, 0x88,
    0xa0, 0xa8, 0xa7, 0x8, 0x87, 0x90, 0xa8, 0x1c,
    0xdd, 0x80,

    /* U+0065 "e" */
    0xb, 0xeb, 0x7, 0xa0, 0xa6, 0x9e, 0xcd, 0x77,
    0xa0, 0x21, 0xa, 0xec, 0x20,

    /* U+0066 "f" */
    0x9, 0xe2, 0xf, 0x10, 0xaf, 0xc0, 0x1f, 0x0,
    0x1f, 0x0, 0x1f, 0x0, 0x1f, 0x0,

    /* U+0067 "g" */
    0x1c, 0xec, 0x87, 0xa0, 0x98, 0x97, 0x8, 0x87,
    0xa0, 0x98, 0x1c, 0xed, 0x80, 0x30, 0xb6, 0x1c,
    0xeb, 0x0,

    /* U+0068 "h" */
    0x79, 0x0, 0x7, 0x90, 0x0, 0x7c, 0xdd, 0x17,
    0xb0, 0xb6, 0x79, 0x9, 0x77, 0x90, 0x97, 0x79,
    0x9, 0x70,

    /* U+0069 "i" */
    0x59, 0x0, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b,

    /* U+006A "j" */
    0x5, 0x90, 0x0, 0x6, 0xa0, 0x6a, 0x6, 0xa0,
    0x6a, 0x6, 0xa0, 0x7a, 0x4e, 0x40,

    /* U+006B "k" */
    0x7a, 0x0, 0x7, 0xa0, 0x0, 0x7a, 0x3e, 0x27,
    0xbe, 0x40, 0x7f, 0xe1, 0x7, 0xb7, 0xb0, 0x7a,
    0xc, 0x60,

    /* U+006C "l" */
    0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b, 0x6b,

    /* U+006D "m" */
    0x7c, 0xdd, 0x8e, 0xc0, 0x7b, 0xb, 0x90, 0xe3,
    0x79, 0xa, 0x70, 0xc4, 0x79, 0xa, 0x70, 0xc4,
    0x79, 0xa, 0x70, 0xc4,

    /* U+006E "n" */
    0x7c, 0xdd, 0x27, 0xb0, 0xb6, 0x79, 0x9, 0x77,
    0x90, 0x97, 0x79, 0x9, 0x70,

    /* U+006F "o" */
    0xb, 0xeb, 0x17, 0xa0, 0x89, 0xa7, 0x5, 0xb7,
    0xa0, 0x89, 0xb, 0xec, 0x10,

    /* U+0070 "p" */
    0x7d, 0xcd, 0x17, 0xa0, 0x99, 0x79, 0x6, 0xb7,
    0xb0, 0x98, 0x7d, 0xdd, 0x17, 0x90, 0x0, 0x79,
    0x0, 0x0,

    /* U+0071 "q" */
    0x1c, 0xec, 0x88, 0xa0, 0xa8, 0xa7, 0x9, 0x87,
    0xa0, 0xa8, 0x1c, 0xed, 0x80, 0x0, 0x98, 0x0,
    0x9, 0x80,

    /* U+0072 "r" */
    0x0, 0x0, 0x7d, 0xf0, 0x7b, 0x0, 0x79, 0x0,
    0x79, 0x0, 0x79, 0x0,

    /* U+0073 "s" */
    0x2c, 0xea, 0x7, 0xa0, 0x81, 0x1a, 0xd8, 0x5,
    0x50, 0xe3, 0x2c, 0xda, 0x0,

    /* U+0074 "t" */
    0x3e, 0xd, 0xf9, 0x3e, 0x3, 0xe0, 0x2e, 0x0,
    0xda,

    /* U+0075 "u" */
    0x79, 0x9, 0x77, 0x90, 0x97, 0x79, 0x9, 0x77,
    0xb0, 0xb7, 0x2d, 0xed, 0x70,

    /* U+0076 "v" */
    0xb5, 0xe, 0x26, 0xa3, 0xd0, 0x1e, 0x78, 0x0,
    0xbe, 0x20, 0x6, 0xd0, 0x0,

    /* U+0077 "w" */
    0xb4, 0x3e, 0xa, 0x67, 0x87, 0xf2, 0xd2, 0x3b,
    0xc7, 0x8d, 0x0, 0xec, 0x1f, 0x90, 0xa, 0x70,
    0xc5, 0x0,

    /* U+0078 "x" */
    0x8a, 0x2e, 0x10, 0xec, 0x70, 0x8, 0xf0, 0x0,
    0xeb, 0x80, 0x99, 0x1e, 0x20,

    /* U+0079 "y" */
    0xc5, 0xf, 0x27, 0xa4, 0xd0, 0x1e, 0x97, 0x0,
    0xcf, 0x20, 0x7, 0xd0, 0x0, 0x87, 0x0, 0x8d,
    0x10, 0x0,

    /* U+007A "z" */
    0x8e, 0xef, 0x10, 0xa, 0x80, 0x6, 0xd0, 0x2,
    0xe2, 0x0, 0x9f, 0xee, 0x30,

    /* U+007B "{" */
    0x3, 0x90, 0xc3, 0xe, 0x10, 0xe0, 0xb9, 0x0,
    0xe0, 0xe, 0x10, 0xc3, 0x3, 0x90,

    /* U+007C "|" */
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x36,

    /* U+007D "}" */
    0x93, 0x3, 0xc0, 0x1e, 0x0, 0xe0, 0x9, 0xb0,
    0xe0, 0x1e, 0x3, 0xc0, 0x93, 0x0,

    /* U+007E "~" */
    0x1c, 0xc3, 0x76, 0x56, 0x3c, 0xc1,

    /* U+00B0 "°" */
    0x1a, 0x56, 0x4a, 0x1a, 0x60,

    /* U+00B7 "·" */
    0x0, 0x4b, 0x1,

    /* U+2013 "–" */
    0x4e, 0xee, 0xe2,

    /* U+2014 "—" */
    0x6e, 0xee, 0xee, 0x70,

    /* U+2026 "…" */
    0x1, 0x1, 0x1, 0x4b, 0x4b, 0x5a
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 36, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 39, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 47, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 12, .adv_w = 88, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 33, .adv_w = 82, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 56, .adv_w = 106, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 81, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 102, .adv_w = 24, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 105, .adv_w = 50, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 122, .adv_w = 51, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 139, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 147, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 160, .adv_w = 32, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 163, .adv_w = 47, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 165, .adv_w = 40, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 57, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 82, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 321, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 339, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 38, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 362, .adv_w = 34, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 369, .adv_w = 73, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 379, .adv_w = 81, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 387, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 70, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 418, .adv_w = 129, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 454, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 475, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 496, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 538, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 79, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 595, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 616, .adv_w = 41, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 80, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 78, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 126, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 708, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 729, .adv_w = 99, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 750, .adv_w = 92, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 771, .adv_w = 99, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 798, .adv_w = 90, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 87, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 840, .adv_w = 87, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 861, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 882, .adv_w = 93, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 903, .adv_w = 127, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 931, .adv_w = 91, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 952, .adv_w = 88, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 973, .adv_w = 87, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 994, .adv_w = 39, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1008, .adv_w = 60, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1022, .adv_w = 39, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1031, .adv_w = 62, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1039, .adv_w = 65, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1042, .adv_w = 46, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 1045, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1058, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1076, .adv_w = 75, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1089, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1107, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1120, .adv_w = 51, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1134, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1152, .adv_w = 80, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1170, .adv_w = 37, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1177, .adv_w = 36, .box_w = 3, .box_h = 9, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 1191, .adv_w = 75, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1209, .adv_w = 37, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1216, .adv_w = 125, .box_w = 8, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1236, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1249, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1262, .adv_w = 81, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1280, .adv_w = 82, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1298, .adv_w = 51, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1310, .adv_w = 74, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1323, .adv_w = 48, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1332, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1345, .adv_w = 71, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1358, .adv_w = 107, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1376, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1389, .adv_w = 70, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1407, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1420, .adv_w = 48, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1434, .adv_w = 36, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1442, .adv_w = 48, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1456, .adv_w = 96, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1462, .adv_w = 55, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1467, .adv_w = 41, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1470, .adv_w = 93, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1473, .adv_w = 111, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1477, .adv_w = 102, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x7, 0x1f63, 0x1f64, 0x1f76
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 8055, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 5, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    1, 53,
    3, 3,
    3, 8,
    3, 34,
    3, 66,
    3, 68,
    3, 69,
    3, 70,
    3, 72,
    3, 78,
    3, 79,
    3, 80,
    3, 81,
    3, 82,
    3, 84,
    3, 88,
    8, 3,
    8, 8,
    8, 34,
    8, 66,
    8, 68,
    8, 69,
    8, 70,
    8, 72,
    8, 78,
    8, 79,
    8, 80,
    8, 81,
    8, 82,
    8, 84,
    8, 88,
    9, 55,
    9, 56,
    9, 58,
    13, 3,
    13, 8,
    15, 3,
    15, 8,
    16, 16,
    34, 3,
    34, 8,
    34, 32,
    34, 36,
    34, 40,
    34, 48,
    34, 50,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 78,
    34, 79,
    34, 80,
    34, 81,
    34, 85,
    34, 86,
    34, 87,
    34, 88,
    34, 90,
    34, 91,
    35, 53,
    35, 55,
    35, 58,
    36, 10,
    36, 53,
    36, 62,
    36, 94,
    37, 13,
    37, 15,
    37, 34,
    37, 53,
    37, 55,
    37, 57,
    37, 58,
    37, 59,
    37, 100,
    38, 53,
    38, 68,
    38, 69,
    38, 70,
    38, 71,
    38, 72,
    38, 80,
    38, 82,
    38, 86,
    38, 87,
    38, 88,
    38, 90,
    39, 13,
    39, 15,
    39, 34,
    39, 43,
    39, 53,
    39, 66,
    39, 68,
    39, 69,
    39, 70,
    39, 72,
    39, 80,
    39, 82,
    39, 83,
    39, 86,
    39, 87,
    39, 90,
    39, 100,
    41, 34,
    41, 53,
    41, 57,
    41, 58,
    42, 34,
    42, 53,
    42, 57,
    42, 58,
    43, 34,
    44, 14,
    44, 36,
    44, 40,
    44, 48,
    44, 50,
    44, 68,
    44, 69,
    44, 70,
    44, 72,
    44, 80,
    44, 82,
    44, 86,
    44, 87,
    44, 88,
    44, 90,
    44, 98,
    44, 99,
    45, 3,
    45, 8,
    45, 34,
    45, 36,
    45, 40,
    45, 48,
    45, 50,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 86,
    45, 87,
    45, 88,
    45, 90,
    46, 34,
    46, 53,
    46, 57,
    46, 58,
    47, 34,
    47, 53,
    47, 57,
    47, 58,
    48, 13,
    48, 15,
    48, 34,
    48, 53,
    48, 55,
    48, 57,
    48, 58,
    48, 59,
    48, 100,
    49, 13,
    49, 15,
    49, 34,
    49, 43,
    49, 57,
    49, 59,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    49, 82,
    49, 85,
    49, 87,
    49, 90,
    49, 100,
    50, 53,
    50, 55,
    50, 56,
    50, 58,
    51, 53,
    51, 55,
    51, 58,
    53, 1,
    53, 13,
    53, 14,
    53, 15,
    53, 34,
    53, 36,
    53, 40,
    53, 43,
    53, 48,
    53, 50,
    53, 52,
    53, 53,
    53, 55,
    53, 56,
    53, 58,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    53, 98,
    53, 99,
    53, 100,
    54, 34,
    55, 10,
    55, 13,
    55, 14,
    55, 15,
    55, 34,
    55, 36,
    55, 40,
    55, 48,
    55, 50,
    55, 62,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 80,
    55, 82,
    55, 83,
    55, 86,
    55, 87,
    55, 90,
    55, 94,
    55, 98,
    55, 99,
    55, 100,
    56, 10,
    56, 13,
    56, 14,
    56, 15,
    56, 34,
    56, 53,
    56, 62,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 72,
    56, 80,
    56, 82,
    56, 83,
    56, 86,
    56, 94,
    56, 98,
    56, 99,
    56, 100,
    57, 14,
    57, 36,
    57, 40,
    57, 48,
    57, 50,
    57, 55,
    57, 68,
    57, 69,
    57, 70,
    57, 72,
    57, 80,
    57, 82,
    57, 86,
    57, 87,
    57, 90,
    57, 98,
    57, 99,
    58, 7,
    58, 10,
    58, 11,
    58, 13,
    58, 14,
    58, 15,
    58, 34,
    58, 36,
    58, 40,
    58, 43,
    58, 48,
    58, 50,
    58, 52,
    58, 53,
    58, 54,
    58, 55,
    58, 56,
    58, 57,
    58, 58,
    58, 62,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 71,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 85,
    58, 86,
    58, 87,
    58, 89,
    58, 90,
    58, 91,
    58, 94,
    58, 98,
    58, 99,
    58, 100,
    59, 34,
    59, 36,
    59, 40,
    59, 48,
    59, 50,
    59, 68,
    59, 69,
    59, 70,
    59, 72,
    59, 80,
    59, 82,
    59, 86,
    59, 87,
    59, 88,
    59, 90,
    60, 43,
    60, 54,
    66, 3,
    66, 8,
    66, 87,
    66, 90,
    67, 3,
    67, 8,
    67, 87,
    67, 89,
    67, 90,
    67, 91,
    68, 3,
    68, 8,
    70, 3,
    70, 8,
    70, 87,
    70, 90,
    71, 3,
    71, 8,
    71, 10,
    71, 62,
    71, 68,
    71, 69,
    71, 70,
    71, 72,
    71, 82,
    71, 94,
    73, 3,
    73, 8,
    76, 68,
    76, 69,
    76, 70,
    76, 72,
    76, 82,
    78, 3,
    78, 8,
    79, 3,
    79, 8,
    80, 3,
    80, 8,
    80, 87,
    80, 89,
    80, 90,
    80, 91,
    81, 3,
    81, 8,
    81, 87,
    81, 89,
    81, 90,
    81, 91,
    83, 3,
    83, 8,
    83, 13,
    83, 15,
    83, 66,
    83, 68,
    83, 69,
    83, 70,
    83, 71,
    83, 72,
    83, 80,
    83, 82,
    83, 85,
    83, 87,
    83, 88,
    83, 90,
    83, 100,
    85, 80,
    87, 3,
    87, 8,
    87, 13,
    87, 15,
    87, 66,
    87, 68,
    87, 69,
    87, 70,
    87, 71,
    87, 72,
    87, 80,
    87, 82,
    87, 100,
    88, 13,
    88, 15,
    88, 100,
    89, 68,
    89, 69,
    89, 70,
    89, 72,
    89, 80,
    89, 82,
    90, 3,
    90, 8,
    90, 13,
    90, 15,
    90, 66,
    90, 68,
    90, 69,
    90, 70,
    90, 71,
    90, 72,
    90, 80,
    90, 82,
    90, 100,
    91, 68,
    91, 69,
    91, 70,
    91, 72,
    91, 80,
    91, 82,
    92, 43,
    92, 54,
    100, 3,
    100, 8
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -4, -3, -3, -8, -4, -4, -4, -4,
    -4, -1, -1, -6, -1, -4, -6, 1,
    -3, -3, -8, -4, -4, -4, -4, -4,
    -1, -1, -6, -1, -4, -6, 1, 1,
    3, 2, -20, -20, -20, -20, -17, -8,
    -8, -6, -1, -1, -1, -1, -8, -1,
    -5, -3, -11, -3, -3, -1, -3, -1,
    -1, -4, -2, -4, 1, -2, -2, -4,
    -2, -2, -1, -1, -9, -9, -1, -6,
    -2, -2, -3, -2, -9, 1, -1, -1,
    -1, -1, -1, -1, -1, -1, -2, -2,
    -2, -19, -19, -13, -15, 1, -2, -1,
    -1, -1, -1, -1, -1, -2, -2, -2,
    -2, -19, 1, -2, 1, -2, 1, -2,
    1, -2, -2, -12, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -3,
    -4, -3, -12, -12, -20, -20, 1, -4,
    -4, -4, -4, -14, -2, -14, -7, -20,
    -1, -9, -4, -9, 1, -2, 1, -2,
    1, -2, 1, -2, -9, -9, -1, -6,
    -2, -2, -3, -2, -9, -28, -28, -13,
    -13, -4, -3, -1, -1, -1, -1, -1,
    -1, -1, 1, 1, 1, -28, -2, -2,
    -1, -2, -4, -1, -3, -4, -18, -19,
    -18, -8, -2, -2, -15, -2, -2, -1,
    1, 1, 1, 1, -12, -6, -6, -6,
    -6, -6, -6, -15, -6, -6, -5, -5,
    -5, -6, -3, -5, -6, -4, -19, -19,
    -18, -2, 1, -15, -11, -15, -5, -1,
    -1, -1, -1, 1, -3, -3, -3, -3,
    -3, -3, -3, -2, -2, -1, -1, 1,
    -11, -11, -15, 1, -10, -4, -10, -3,
    1, 1, -2, -2, -2, -2, -2, -2,
    -2, -1, -1, 1, -4, -4, -10, -11,
    -2, -2, -2, -2, 1, -2, -2, -2,
    -2, -1, -2, -1, -2, -2, -11, -11,
    -2, 1, -3, -16, -11, -16, -11, -2,
    -2, -7, -2, -2, -1, 1, -7, 1,
    1, 1, 1, 1, -4, -5, -5, -5,
    -2, -5, -3, -3, -5, -3, -5, -3,
    -4, -2, -3, -1, -2, -1, -2, 1,
    -11, -11, -16, 1, -2, -2, -2, -2,
    -1, -1, -1, -1, -1, -1, -1, -2,
    -2, -2, -1, -1, -1, -1, -1, -1,
    -2, -2, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 1, 1, 1, 1,
    -2, -2, -2, -2, -2, 1, -6, -6,
    -1, -1, -1, -1, -1, -6, -6, -6,
    -6, -6, -6, -1, -1, -1, -1, -2,
    -2, -1, -1, -1, -1, 1, 1, -12,
    -12, -2, -1, -1, -1, 1, -1, -3,
    -1, 4, 1, 1, 1, -12, -2, 1,
    1, -12, -12, -1, -1, -1, -1, 1,
    -1, -1, -1, -12, -9, -9, -9, -1,
    -1, -1, -1, -3, -1, 1, 1, -12,
    -12, -1, -1, -1, -1, 1, -1, -1,
    -1, -12, -1, -1, -1, -1, -1, -1,
    -1, -1, -20, -20
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 460,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_rm9 = {
#else
lv_font_t ui_font_rm9 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_RM9*/

