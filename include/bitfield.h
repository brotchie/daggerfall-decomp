/* bitfield.h: the lifter's views of single bits and bit groups (bfW_B_N: N bits from bit B of a
 * W-bit unit). A test or store through one compiles to the byte or dword bit operation the original
 * code has, where a mask on the member does not (docs/structs.md, "Bit tests"). */
#ifndef BITFIELD_H
#define BITFIELD_H

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_2 { unsigned char f:2; };
struct bf8_0_4 { unsigned char f:4; };
struct bf8_0_5 { unsigned char f:5; };
struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_1_5 { unsigned char _:1; unsigned char f:5; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
struct bf32_30_1 { unsigned _:30; unsigned f:1; };
struct bf32_31_1 { unsigned _:31; unsigned f:1; };

#endif
