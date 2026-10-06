/* xstr.h: XnGine's string and array helpers (src/engine/str.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Copies that stop at a delimiter, searches of byte, word and dword arrays, fills, and
     numbers to text and back. The game's C calls most of them one at a time (popups, the
     class maker, the inventory, macros); the rest are XnGine's general library, linked in
     whole and never called (marked dead).

   Counts
     A count is a number of elements. The helpers that test the count after the first element
     (do-while) take a count of 0 as 2^32 (Q-STR-02); each says so.

   Globals (object 2): xn_pow10_table (1, 10 ... 10^9, at 0x153804; the dword before it is
   xn_rand_seed, which xn_str_from_int reads as the table's entry -1, Q-STR-01).

   Quirks (docs/engine/quirks.md): Q-STR-01 (from_int with 1 or 0 digits), Q-STR-02 (a count
   of 0 is 2^32), Q-STR-03 (append_char writes no terminator), Q-STR-04 (copy_alnum folds no
   case), Q-STR-05 (find_char's "not found" is index 0), Q-STR-06 (copy_word_max80 compares
   signed). */
#ifndef XSTR_H
#define XSTR_H

#include "xngine.h"

/* 1 10 100 ... 1000000000: the place values xn_str_from_int divides by (0x153804) */
extern u32 xn_pow10_table[10];

/* ---- copies ------------------------------------------------------------------------------ */

/* Dead: *dst = *src, one dword. */
void xn_str_copy_dword(u32 *dst, const u32 *src);

/* Copies one line of src to dst: up to a NUL or a CR, which becomes the terminator. Returns
   src past that character: the next line. info_popup_update splits popup text with it. */
char *xn_str_copy_line(char *dst, const char *src);

/* Copies the leading characters of src above ' ' (a word; bytes from 80h count as above) to
   dst and terminates it; returns how many. macro_1hn_hero1_name: the player's first name. */
s32 xn_str_copy_word(char *dst, const char *src);

/* Copies the leading letters and digits of src to dst and terminates it; returns how many.
   Q-STR-04: lower case stays lower case (the asm's case mask lands on capitals only).
   parse_expand reads a macro name with it. */
s32 xn_str_copy_alnum(char *dst, const char *src);

/* Copies src to dst up to delim or a NUL, and terminates it. class_question_show splits the
   questions at '.' and 252. */
void xn_str_copy_until(char *dst, const char *src, char delim);

/* strcpy, with the source first: src to dst with its terminator. The movie player copies its
   file name with it. */
void xn_str_copy(const char *src, char *dst);

/* Dead: copies src to dst up to the first byte <= ' ', at most 80, and terminates it.
   Q-STR-06: the compare is signed, so bytes from 80h stop it too. */
void xn_str_copy_word_max80(char *dst, const char *src);

/* ---- searches ---------------------------------------------------------------------------- */

/* Dead: a pointer to the first ch in the n bytes at s, or 0 at a NUL or after n bytes. */
const char *xn_str_find_char_n(const char *s, char ch, u32 n);

/* A pointer to the first of count dwords equal to value, or 0. The inventory (equipped
   list), links_trigger and magic_items_add_cb look up item handles with it (9 game sites). */
u32 *xn_str_find_u32(u32 *array, u32 value, u32 count);

/* A pointer to the first of count words equal to value, or 0 (Q-STR-02: count 0 is 2^32).
   Texture archives by climate, face records (7 game sites). */
u16 *xn_str_find_u16(u16 *array, u16 value, u32 count);

/* How many of the n bytes at p are not 0 (Q-STR-02: n = 0 is 2^32). */
s32 xn_str_count_nonzero(const u8 *p, u32 n);

/* s past n fields that each end in delim: a pointer to the start of field n, or 0 when a NUL
   comes first (delim 0 finds nothing). */
const char *xn_str_skip_fields(const char *s, char delim, s32 n);

/* A pointer to the first place in the n bytes at p where the two bytes of pair (low byte
   first) are, at any byte offset; 0 if none (Q-STR-02: n = 0 is 2^32). */
u8 *xn_str_find_byte_pair(u8 *p, u16 pair, u32 n);

/* A pointer to the first of the n bytes at p that is not 0, or 0. */
u8 *xn_str_find_nonzero(u8 *p, u32 n);

/* Dead: strlen. */
s32 xn_str_length(const char *s);

/* Dead: the index of the first ch in the string s (ch = 0 finds the terminator). Q-STR-05:
   0 when it is not there, the same as at index 0. */
s32 xn_str_find_char(const char *s, char ch);

/* ---- fills and edits ---------------------------------------------------------------------- */

/* Fills bytes / 2 words at dst with value. classmaker_run, flc_decode_ss2. */
void xn_str_fill_u16(u16 *dst, u16 value, u16 bytes);

/* dst[k] = first + k (a byte) for k < n: an identity remap table (Q-STR-02: n = 0 is 2^32).
   11 game sites. */
void xn_str_fill_ascending(u8 *dst, u8 first, u32 n);

/* Dead: a second copy of xn_str_fill_ascending. */
void xn_str_fill_ascending_v2(u8 *dst, u8 first, u32 n);

/* Appends ch to the string s by writing it over the terminator. Q-STR-03: no new terminator
   (the callers' buffers are zeroed). */
void xn_str_append_char(char *s, char ch);

/* Dead: inserts c at s[at], moving the rest of the string, its terminator too, right. */
void xn_str_insert_char(char *s, s32 at, char c);

/* Dead: removes s[at], moving the rest of the string left. */
void xn_str_delete_char(char *s, s32 at);

/* Dead: 'A'..'Z' to lower case; any other byte as it is. */
u8 xn_str_char_lower(u8 c);

/* Dead: 'a'..'z' to upper case; any other byte as it is. */
u8 xn_str_char_upper(u8 c);

/* Dead: an empty function between the string helpers. */
void xn_str_nop(void);

/* ---- numbers ------------------------------------------------------------------------------- */

/* Writes value as ndigits decimal digits at dst, after a '-' when it is negative; no
   terminator. A value with more digits gives a first "digit" past '9'. The file names'
   numbers: FONT0000 (4 digits), SHADE.000 (3), HAZE.000 (3). Q-STR-01: 1 digit writes nothing,
   and 0 digits writes one digit of value divided by xn_rand_seed. */
void xn_str_from_int(s32 value, char *dst, s32 ndigits);

/* atoi: an optional sign, then decimal digits (32-bit, wrapping). The head tracker's version
   reply. */
s32 xn_str_to_int(const char *s);

#endif
