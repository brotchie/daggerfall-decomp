/* xhelmet.h: XnGine's head trackers (src/engine/helmet.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     A front end over three head-tracker drivers: A and B talk to a tracker on a serial port
     (xserial.h), C through the mouse driver's tracker functions (int 33h 60xxh, called in
     real mode through DPMI). The front end opens the configured driver, asks it for samples,
     and smooths them: a ring of the last samples averaged. init_game_data opens driver A or
     B when the config's 'helmet' is set; nothing in the game polls one, and no config of the
     game sets it.

   Angles
     A sample is three angles (pitch, yaw, roll) in the tracker's units; the smoothing works
     in 16ths: the sum of the last n samples over 16n. A yaw more than 4000h from the
     newest is taken half a turn (8000h) nearer.

   Drivers
     cfg_helmet picks the driver: a byte offset into the asm's tables of driver routines (0 A,
     4 B, 8 C), -1 after a failed open. The C keeps its own table of the drivers' C routines
     (struct xn_helmet_driver), indexed by cfg_helmet / 4.
     A: 'R' reset, 'G' request, 'H'; 6-byte binary records it never decodes into angles.
     B: '!R' reset, '!M2,C,B' mode, '!V' version, '!' and 'S' start; 8-byte packets (FFh, the
        three angles as big-endian words, a checksum).
     C: int 33h 607Fh detect, 6003h/6004h read and write the configuration in a DOS block,
        6005h read.

   Globals (object 2): the front end's (xn_helmet_port, xn_helmet_active, xn_helmet_smoothing,
   the smoothed xn_helmet_pitch / yaw / roll, which cast_fire_missile adds when active, the
   ring); each driver's (A's port, record and angles; B's command strings and yaw sign; C's
   real-mode register block and DOS buffer). The game's: cfg_helmet. The smoothing sums in
   the pick code's scratch point (xn_pick_view_x, xn_pick_view_y, pick_distance: group D's).

   Quirks (docs/engine/quirks.md): Q-HELMET-01 (the sums land in the pick's scratch, which the
   game reads), Q-HELMET-02 (driver A's read gives no angles), Q-HELMET-03 (driver B's packet
   search goes on looking for the bad checksum), Q-HELMET-04 (driver A's read counts port 1's
   ring), Q-HELMET-05 (the version scan starts at the ring's start), Q-HELMET-06 (driver C's
   read passes its caller's EBX to the mouse driver). */
#ifndef XHELMET_H
#define XHELMET_H

#include "xngine.h"
#include "xpc.h"

extern s32 xn_helmet_port;                  /* the COM port (the game passes 1) */
extern s32 cfg_helmet;                      /* the driver: 0 A, 4 B, 8 C; -1 after a failure */
extern u8 xn_helmet_active;
extern s32 xn_helmet_smoothing;             /* samples averaged, at most 7 (the game sets 4) */
extern s32 xn_helmet_pitch, xn_helmet_yaw, xn_helmet_roll;
extern u32 xn_helmet_ring_pos;              /* byte offset of the next sample */
extern xn_vec3 xn_helmet_ring[7];           /* the last samples */
extern s32 xn_pick_view_x, xn_pick_view_y, pick_distance;  /* (group D's scratch point) */

extern s32 xn_helmet_a_port;
extern char xn_helmet_a_record[24];         /* the last record read */
extern s32 xn_helmet_a_angles[3];           /* cleared by the open; nothing else uses them */
extern s32 xn_helmet_b_yaw_sign;            /* -1 from firmware 1.003 */
extern u8 xn_helmet_b_str_reset[];          /* '!R' CR, FFh-terminated */
extern u8 xn_helmet_b_str_version[];        /* '!V' CR */
extern u8 xn_helmet_b_str_mode[];           /* '!M2,C,B' CR (0x160F08) */
extern u8 xn_helmet_b_str_start[];          /* 'S' */
extern u8 xn_helmet_b_str_bang[];           /* '!' CR (0x160F13) */
extern xn_dpmi_rm_regs xn_helmet_c_rm_regs;
extern u32 xn_helmet_c_dos_selector;        /* driver C's DOS block */
extern u8 *xn_helmet_c_dos_buffer;          /* and its linear address */

/* A driver's routines, as the front end calls them */
typedef struct xn_helmet_driver {
    int (*open)(s32 port);                  /* 1 when the tracker answered */
    void (*close)(void);
    int (*read)(xn_vec3 *angles);           /* 1 with a sample in *angles */
    void (*request)(void);                  /* asks for a sample */
} xn_helmet_driver;

/* ---- the front end ------------------------------------------------------------------------ */

/* init_game_data (2 game sites): opens driver (0 or 4: A or B) on port; when it fails the
   driver becomes -1. Returns 1 when open. */
s32 xn_helmet_open(s32 port, s32 driver);

/* Dead: closes the driver, then the serial port (also for driver C, which has none). */
void xn_helmet_close(void);

/* Dead: asks the driver for a sample. */
void xn_helmet_request(void);

/* Dead: reads the driver; a sample is smoothed into xn_helmet_pitch, yaw and roll. */
void xn_helmet_poll(void);

/* Puts the sample s into the ring and, with smoothing on, makes s the average of the last
   xn_helmet_smoothing samples in 16ths (yaws across the wrap taken half a turn nearer).
   Q-HELMET-01: the sums are kept in the pick's scratch point. A smoothing of 0 leaves s. */
void xn_helmet_smooth(xn_vec3 *s);

/* ---- driver A ----------------------------------------------------------------------------- */

s32 xn_helmet_a_open(s32 port);             /* opens port at 19200 8N1, 'R', 'G': 1 when the
                                               'G' went out */
void xn_helmet_a_close(void);               /* nothing */
s32 xn_helmet_a_reset(void);                /* sends 'R': 1 when sent */
s32 xn_helmet_a_send_h(void);               /* sends 'H': 1 when sent */
s32 xn_helmet_a_request(void);              /* sends 'G': 1 when sent */
/* Dead: waits (16M polls) for 18 bytes in port 1's ring (Q-HELMET-04) and reads them into
   the record, without CR and LF; 0, or -1 on the timeout. */
s32 xn_helmet_a_read_text(void);
/* The same for a 6-byte record (64K polls); 0, or -1 on the timeout. */
s32 xn_helmet_a_read(void);
s32 xn_helmet_a_fail_stub(void);            /* dead: -1 */
s32 xn_helmet_a_send(u8 byte);              /* 1 when sent */

/* ---- driver B ----------------------------------------------------------------------------- */

s32 xn_helmet_b_open(s32 port);             /* reset, mode, version, start: 1 when open (the
                                               port closed again on a failure) */
void xn_helmet_b_close(void);               /* sends '!R' */
s32 xn_helmet_b_reset(void);                /* '!R' until an 'O', 8 tries: 1 when it came */
void xn_helmet_b_send_bang(void);           /* sends '!' CR */
void xn_helmet_b_start(void);               /* sends 'S' */
/* '!V', then up to 18 BIOS ticks for 60 bytes: 1 when they came */
s32 xn_helmet_b_request_version(void);
void xn_helmet_b_stub(void);                /* dead: nothing */
/* The firmware version in the reply ('M'...'F' major '.' minor): major * 1000 + the number
   at F + 5, or 0 when there is none. Q-HELMET-05: it scans the port's ring from its start
   (511 bytes), not from its head. */
s32 xn_helmet_b_parse_version(void);
/* The packet search in a port's ring: back from index start, 128 bytes at most, for an FFh
   whose six bytes after it sum (8-bit, the FFh included) to the byte after them. Returns the
   packet's index, or -1. Q-HELMET-03: after a bad checksum the search stays on the same
   byte, now looking for that checksum's value instead of FFh. *mark: the value it ended
   looking for; *summed: 1 when it checked a candidate. */
s32 xn_helmet_b_find_packet(const u8 *ring, u32 start, u8 *mark, s32 *summed);
/* The angles of the newest packet in the ring (searched from 8 bytes before its write
   index): pitch and roll the negated words, yaw the word times xn_helmet_b_yaw_sign. 1 when
   found. */
s32 xn_helmet_b_read(xn_vec3 *angles);
void xn_helmet_b_send_str(const u8 *s);     /* bytes up to FFh, each once the line is idle */
s32 xn_helmet_b_wait_ok(void);              /* a byte within 9 ticks, and it is 'O' */
s32 xn_helmet_b_wait_rx(void);              /* any byte within 9 BIOS ticks */

/* ---- driver C ----------------------------------------------------------------------------- */

s32 xn_helmet_c_open(void);                 /* detects the tracker, sets its configuration:
                                               1 when open */
s32 xn_helmet_c_close(void);                /* frees the DOS block: 1 when DPMI failed */
void xn_helmet_c_reset_noop(void);
void xn_helmet_c_request_noop(void);
void xn_helmet_c_cmd2_noop(void);
/* int 33h 6005h (with bx: Q-HELMET-06): when the driver answers, the angles from the DOS
   block, halved, pitch and yaw negated; 1 then. */
s32 xn_helmet_c_read(u32 bx, xn_vec3 *angles);
/* int 33h in real mode (DPMI 0300h) with EAX, EBX, ECX and EDX in the register block; the
   driver's answer is left there */
void xn_helmet_c_int33(u32 eax, u32 ebx, u32 ecx, u32 edx);

#endif
