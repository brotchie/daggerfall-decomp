/* xhelmet.h: XnGine's head trackers (src/engine/helmet.c; see xngine.h): a front end with a
   table of three drivers (A and B on the serial driver, xserial.h; C through the mouse
   driver's int 33h 60xxh functions), and a ring that averages the last samples. init_game_data
   opens driver A or B when the config's 'helmet' is set; nothing polls them, and the game
   never ran them. Driver B's command strings (0x160F00..0x160F15) are data, not code.

   A driver's routines have a register interface (the front end calls them through its
   tables): EAX the port for open, CF set on failure, the angles in EAX, EDX, EBX from read.
   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue. */
#ifndef XHELMET_H
#define XHELMET_H

#include "xngine.h"
#include "xsysutil.h"

extern s32 xn_helmet_port;                  /* the COM port (the game passes 1) */
extern s32 cfg_helmet;                      /* the driver: a byte offset into the slot
                                               tables (0 A, 4 B, 8 C), -1 after a failure */
extern u8 xn_helmet_active;
extern s32 xn_helmet_smoothing;             /* samples averaged, at most 7 (the game sets 4) */
extern s32 xn_helmet_pitch, xn_helmet_yaw, xn_helmet_roll;
/* The drivers' routines, 3 a row (A, B, C), by cfg_helmet */
extern void (*xn_helmet_drv_open[3])(void);
extern void (*xn_helmet_drv_close[3])(void);
extern void (*xn_helmet_drv_read[3])(void);
extern void (*xn_helmet_drv_request[3])(void);
extern u32 xn_helmet_ring_pos;              /* byte offset of the next sample */
extern xn_vec3 xn_helmet_ring[7];           /* the last samples */
/* The smoothing sums its samples in the pick code's scratch point (group 3's globals) */
extern s32 xn_pick_view_x, xn_pick_view_y, pick_distance;

extern s32 xn_helmet_a_port;
extern char xn_helmet_a_record[24];         /* the last record read */
extern s32 xn_helmet_a_angles[3];           /* cleared by the open; nothing else uses them */
extern s32 xn_helmet_b_yaw_sign;            /* -1 from firmware 1.003 */
extern u8 xn_helmet_b_str_reset[];          /* '!R' CR, FFh-terminated */
extern u8 xn_helmet_b_str_version[];        /* '!V' CR */
extern u8 xn_helmet_b_str_mode[];           /* '!M2,C,B' CR (0x160F08) */
extern u8 xn_helmet_b_str_start[];          /* 'S' */
extern u8 xn_helmet_b_str_bang[];           /* '!' CR (0x160F13) */

/* A DPMI real-mode register block (DPMI 0300h) */
typedef struct xn_dpmi_rm_regs {
    u32 edi, esi, ebp, reserved, ebx, edx, ecx, eax;
    u16 flags, es, ds, fs, gs, ip, cs, sp, ss;
} xn_dpmi_rm_regs;
extern xn_dpmi_rm_regs xn_helmet_c_rm_regs;
extern u32 xn_helmet_c_dos_selector;        /* driver C's DOS block */
extern u8 *xn_helmet_c_dos_buffer;          /* and its linear address */

/* init_game_data: opens driver (an offset, 0 or 4) on port; when it fails the driver
   becomes -1. Returns 1 when open (the asm's CF clear). */
s32 xn_helmet_open(s32 port, s32 driver);
void xn_helmet_open_r(xn_regs *r);

/* Dead: closes the driver, then the serial port (also for driver C, which has none). */
void xn_helmet_close(void);
#pragma aux xn_helmet_close parm [] modify exact [eax edx];

/* Dead: asks the driver for a sample. */
void xn_helmet_request(void);
#pragma aux xn_helmet_request parm [] modify exact [eax];

/* Dead: reads the driver with the caller's registers (driver A's read returns EDX and EBX
   as they came); a sample is smoothed into xn_helmet_pitch, yaw and roll. Every register is
   kept. */
void xn_helmet_poll_r(xn_regs *r);

/* Puts the sample s into the ring and, with smoothing on, makes it the sum of the last
   xn_helmet_smoothing samples over 16 times their count; a yaw more than 4000h from the
   sample's is taken half a turn (8000h) nearer. Kept from the asm: the sums are kept in the
   pick code's scratch point. */
void xn_helmet_smooth(xn_vec3 *s);
void xn_helmet_smooth_r(xn_regs *r);

/* Driver A ('R' reset, 'G' request, 'H'; 6-byte binary records) */
s32 xn_helmet_a_open(s32 port);             /* 1 when the 'G' went out */
void xn_helmet_a_open_r(xn_regs *r);
void xn_helmet_a_close(void);               /* nothing */
#pragma aux xn_helmet_a_close parm [] modify exact [eax];
s32 xn_helmet_a_reset(void);                /* sends 'R': 1 when sent */
void xn_helmet_a_reset_r(xn_regs *r);
s32 xn_helmet_a_send_h(void);               /* sends 'H' */
void xn_helmet_a_send_h_r(xn_regs *r);
s32 xn_helmet_a_request(void);              /* sends 'G': 0 sent, 1 not */
void xn_helmet_a_request_r(xn_regs *r);
/* Waits (16M loops) for 18 bytes in port 1's ring and reads them into the record, without
   CR and LF; 0, or -1 on the timeout. Kept from the asm: it counts port 1's ring whatever the
   port. */
s32 xn_helmet_a_read_text(void);
/* The same for a 6-byte record (64K loops). Driver A's read slot returns no angles. */
s32 xn_helmet_a_read(void);
void xn_helmet_a_read_r(xn_regs *r);
s32 xn_helmet_a_fail_stub(void);            /* -1 */
s32 xn_helmet_a_send(u8 byte);              /* 0 sent, 1 not */
void xn_helmet_a_send_r(xn_regs *r);

/* Driver B ('!R' reset, '!M2,C,B' mode, '!V' version, '!' and 'S' start; 8-byte packets) */
s32 xn_helmet_b_open(s32 port);             /* 0 open, 1 failed (the port closed again) */
void xn_helmet_b_open_r(xn_regs *r);
void xn_helmet_b_close(void);               /* sends '!R' */
void xn_helmet_b_close_r(xn_regs *r);
s32 xn_helmet_b_reset(void);                /* '!R' until an 'O', 8 tries: 1 when it came */
void xn_helmet_b_reset_r(xn_regs *r);
void xn_helmet_b_send_bang(void);
void xn_helmet_b_send_bang_r(xn_regs *r);
void xn_helmet_b_start(void);
void xn_helmet_b_start_r(xn_regs *r);
/* '!V', then up to 18 BIOS ticks for 60 bytes: 1 when they came */
s32 xn_helmet_b_request_version(void);
void xn_helmet_b_request_version_r(xn_regs *r);
void xn_helmet_b_stub(void);                /* nothing */
#pragma aux xn_helmet_b_stub parm [] modify exact [eax];
/* The firmware version in the reply ('M'...'F' major '.' minor): major * 1000 + the number
   at F + 5, or 0 when there is none. It scans port's ring from its start, not from its head. */
s32 xn_helmet_b_parse_version(void);
/* The angles of the newest packet in the ring (searched back from 8 bytes before the write
   index, 128 bytes at most): FFh, six bytes, their checksum (8-bit, FFh included); pitch and
   roll the negated words, yaw the word times xn_helmet_b_yaw_sign. 1 when found. Kept from
   the asm: after a bad checksum the search goes on for a byte equal to that checksum. */
s32 xn_helmet_b_read(xn_vec3 *angles);
void xn_helmet_b_read_r(xn_regs *r);
void xn_helmet_b_send_str(const u8 *s);     /* bytes up to FFh, each once the line is idle */
void xn_helmet_b_send_str_r(xn_regs *r);
s32 xn_helmet_b_wait_ok(void);              /* a byte within 9 ticks, and it is 'O' */
void xn_helmet_b_wait_ok_r(xn_regs *r);
s32 xn_helmet_b_wait_rx(void);              /* any byte within 9 BIOS ticks */
void xn_helmet_b_wait_rx_r(xn_regs *r);

/* Driver C: the mouse driver's tracker functions (int 33h 607Fh detect, 6003h/6004h read and
   write the configuration in a DOS block, 6005h read) */
s32 xn_helmet_c_open(void);                 /* 0 open, 1 not */
void xn_helmet_c_open_r(xn_regs *r);
s32 xn_helmet_c_close(void);                /* frees the DOS block: DPMI's carry */
void xn_helmet_c_close_r(xn_regs *r);
void xn_helmet_c_reset_noop(void);
#pragma aux xn_helmet_c_reset_noop parm [] modify exact [eax];
void xn_helmet_c_request_noop(void);
#pragma aux xn_helmet_c_request_noop parm [] modify exact [eax];
void xn_helmet_c_cmd2_noop(void);
#pragma aux xn_helmet_c_cmd2_noop parm [] modify exact [eax];
/* int 33h 6005h with the caller's EBX: the angles (halved; pitch and yaw negated) */
void xn_helmet_c_read_r(xn_regs *r);
/* int 33h in real mode (DPMI 0300h) with EAX, EBX, ECX and EDX through the register block;
   the results are left there */
void xn_helmet_c_int33(u32 eax, u32 ebx, u32 ecx, u32 edx);
void xn_helmet_c_int33_r(xn_regs *r);

#endif
