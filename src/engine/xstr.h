/* xstr.h: XnGine's string and array helpers (src/engine/str.c; see xngine.h): copies that
   stop at a delimiter, searches of byte, word and dword arrays, fills, and number <-> text.
   Most are called one at a time from the game's C; the rest are XnGine's general library,
   linked in whole and never called.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XSTR_H
#define XSTR_H

#include "xngine.h"

/* 1 10 100 ... 1000000000: the place values xn_str_from_int divides by (0x153804) */
extern u32 xn_pow10_table[10];

/* Dead: *dst = *src, one dword (the value is left in EDX). */
void xn_str_copy_dword(u32 *dst, const u32 *src);
#pragma aux xn_str_copy_dword parm [eax] [edx] modify exact [edx];

/* Copies one line of src to dst: up to a NUL or a CR, which becomes the terminator. Returns
   src past that character: the next line. info_popup_update splits popup text with it. */
char *xn_str_copy_line(char *dst, const char *src);

/* Copies the characters of src above ' ' (a word) to dst and terminates it; returns how many.
   macro_1hn_hero1_name copies the player's first name. */
s32 xn_str_copy_word(char *dst, const char *src);

/* Copies the leading letters and digits of src to dst and terminates it; returns how many.
   parse_expand reads a macro name with it. */
s32 xn_str_copy_alnum(char *dst, const char *src);

/* Dead: a pointer to the first ch in the n bytes at s, or 0 at a NUL or after n bytes. */
const char *xn_str_find_char_n(const char *s, char ch, u32 n);
#pragma aux xn_str_find_char_n parm [eax] [edx] [ebx] value [eax] modify exact [eax ebx];

/* A pointer to the first of count dwords equal to value, or 0. The inventory (equipped list),
   links_trigger and magic_items_add_cb look up item handles with it. */
u32 *xn_str_find_u32(u32 *array, u32 value, u32 count);

/* A pointer to the first of count words equal to value, or 0 (count 0 means 2^32: the asm
   tests the count after the first word). Texture archives by climate, face records. */
u16 *xn_str_find_u16(u16 *array, u16 value, u32 count);

/* Copies src to dst up to delim or a NUL, and terminates it. class_question_show splits the
   questions at '.' and 252. */
void xn_str_copy_until(char *dst, const char *src, char delim);

/* Fills bytes / 2 words at dst with value (rep stosw). classmaker_run, flc_decode_ss2. */
void xn_str_fill_u16(u16 *dst, u16 value, u16 bytes);
#pragma aux xn_str_fill_u16 parm [eax] [edx] [ebx] modify exact [eax edx ebx];

/* Dead: a copy of xn_str_fill_ascending. */
void xn_str_fill_ascending_v2(u8 *dst, u8 first, u32 n);

/* Appends ch to the string s by writing it over the terminator: it does not write a new one
   (the callers' buffers are zeroed). */
void xn_str_append_char(char *s, char ch);

/* dst[k] = first + k for k < n, in bytes (n = 0 means 2^32): an identity remap table. */
void xn_str_fill_ascending(u8 *dst, u8 first, u32 n);

/* How many of the n bytes at p are not 0 (n = 0 means 2^32). */
s32 xn_str_count_nonzero(const u8 *p, u32 n);

/* s past n fields that end in delim: a pointer to the start of field n, or 0 when a NUL
   comes first. */
const char *xn_str_skip_fields(const char *s, char delim, s32 n);

/* A pointer to the first place in the n bytes at p where the two bytes pair (low byte first)
   are, at any byte offset; 0 if none (n = 0 means 2^32). */
u8 *xn_str_find_byte_pair(u8 *p, u16 pair, u32 n);

/* A pointer to the first of the n bytes at p that is not 0, or 0 (repe scasb). */
u8 *xn_str_find_nonzero(u8 *p, u32 n);

/* Dead: 'A'..'Z' to lower case; any other byte as it is. */
u8 xn_str_char_lower(u8 c);
#pragma aux xn_str_char_lower parm [eax] value [al] modify exact [eax];

/* Dead: 'a'..'z' to upper case; any other byte as it is. */
u8 xn_str_char_upper(u8 c);
#pragma aux xn_str_char_upper parm [eax] value [al] modify exact [eax];

/* strcpy, with the source first: copies src to dst with its terminator. The video player
   (C1500) copies its file name with it. */
void xn_str_copy(const char *src, char *dst);

/* Dead: a lone ret between the string helpers. */
void xn_str_nop(void);
#pragma aux xn_str_nop parm [] modify exact [];

/* Dead: inserts c at s[at], shifting the rest of the string right (its terminator too). */
void xn_str_insert_char(char c, char *s, s32 at);
#pragma aux xn_str_insert_char parm [eax] [esi] [edi] modify exact [eax];

/* Dead: removes s[at], shifting the rest of the string left. */
void xn_str_delete_char(s32 at, char *s);

/* Dead: strlen. */
s32 xn_str_length(const char *s);

/* Dead: the index of the first ch in the string s (ch = 0 finds the terminator); 0 when it is
   not there, the same as at index 0. */
s32 xn_str_find_char(const char *s, char ch);
#pragma aux xn_str_find_char parm [eax] [edx] value [eax] modify exact [eax];

/* Writes value as ndigits decimal digits at dst, after a '-' when it is negative; no
   terminator. The file names' numbers: FONT0000 (4 digits), SHADE.000 (3), haze.000 (3).
   Kept from the asm: 1 digit writes nothing, and 0 digits writes one digit of value divided
   by the dword before the table (xn_rand_seed). */
void xn_str_from_int(s32 value, char *dst, s32 ndigits);
#pragma aux xn_str_from_int parm [eax] [edx] [ebx] modify exact [eax edx];

/* atoi: an optional sign, then decimal digits. The head tracker's version reply (161114). */
s32 xn_str_to_int(const char *s);

/* Dead: copies src to dst up to the first byte <= ' ' (signed: bytes from 80h stop it too),
   at most 80, and terminates it. */
void xn_str_copy_word_max80(const char *src, char *dst);
#pragma aux xn_str_copy_word_max80 parm [esi] [edi] modify exact [eax esi edi];

#endif
