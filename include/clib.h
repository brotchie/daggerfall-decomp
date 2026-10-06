/* clib.h: the library functions the game calls: Watcom's C library, StratosWare MemCheck 3.5
 * and HMI SOS 4.0, with their parameters. Each file used to declare its own, mostly without
 * parameters (`extern int mc_memcpy();`); a pointer passed or returned through one of those
 * loses its top half in the native build (docs/port.md). Under Watcom these prototypes give
 * the calls the code they had. The native build's definitions are in port/shim (port.h sends
 * the DOS file and random-number calls there); the few the host's C library provides take its
 * `size_t` as csize. A file keeps its own `#pragma aux` for the functions that take their
 * arguments on the stack (mc_set_location, func_0009DA1C...).
 *
 * Names follow config/names.csv; some are known to be wrong and stay until the renaming pass:
 * mc_strncpy (0xA0AD9) is MemCheck's strcpy, fprintf (0xA16F8) is sscanf (port.h sends it
 * to port_sscanf), fprintf_2 (0xA31A6) is fscanf. */
#ifndef CLIB_H
#define CLIB_H

#ifdef DAGGER_PORT
typedef unsigned long csize;    /* the host's size_t */
/* the game's view of the C library (strlen returns an int...) differs from the host's builtins */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"
#else
typedef unsigned csize;
#endif

/* ---- Watcom C library -------------------------------------------------------------------- */
extern int abs(int);
extern int atoi(char *);
extern char *itoa(int, char *, int);
extern char *utoa(unsigned, char *, int);
extern int rand(void);
extern void srand(unsigned);
extern void exit(int);
extern int tolower(int);
extern int toupper(int);
extern int printf(char *, ...);

extern int strlen(char *);
extern char *strchr(char *, int);
extern char *strstr(char *, char *);
extern int strcmp(char *, char *);
extern int strncmp(char *, char *, csize);
extern int stricmp(char *, char *);
extern int strnicmp(char *, char *, unsigned);
extern char *memchr(char *, int, csize);
extern int memcmp(void *, void *, csize);

/* io.h, fcntl.h: DOS files (port/shim/dosfile.c) */
extern int open(char *, int, ...);
extern int read(int, void *, unsigned);
extern int write(int, void *, unsigned);
extern int lseek(int, int, int);
extern int close(int);
extern int filelength(int);
extern int unlink(char *);
/* stdio.h: the game never looks inside a FILE */
extern void *fopen(char *, char *);
extern int fclose(void *);
extern int fprintf(char *, char *, ...);        /* sscanf */
extern int fprintf_2(void *, char *, ...);      /* fscanf */

extern unsigned func_000A277F(void *);          /* _msize */

/* ---- StratosWare MemCheck 3.5: the checked calls name the source file and line ------------ */
extern void *mc_malloc(unsigned, char *, int);
extern void mc_free(void *, char *, int);
extern void *mc_memcpy(void *, void *, unsigned, char *, int, int);
extern void *mc_memmove(void *, void *, unsigned, char *, int, int);
extern void *mc_memset(void *, int, unsigned, char *, int, int);
/* MemCheck's strcpy: n is the destination's size (for the check) */
extern char *mc_strncpy(char *, char *, unsigned, char *, int);
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern void func_0009DA1C(int, char *);         /* the API entry under mc_set_location */
extern void func_000A148C(char *, ...);         /* mc_debugf */
extern int func_000A18C3(char *);               /* mc_stack_trace */
extern int func_000A29BA(void *);               /* mc_check */
extern int func_000A2A2B(void);                 /* mc_check_buffers */
extern void func_000A2D9E(void);                /* the default exception report */
extern char *func_000A1054(char *, char *, char *, int, int);  /* mc_strcat */
extern char *func_000A14E8(char *, char *, unsigned, char *, int, int);  /* mc_strncpy */
extern void *func_000A1944(void *, int, unsigned);              /* memset */

/* ---- HMI SOS 4.0 (port/shim/sos.c; its W32 is an int here) ------------------------------- */
struct sos_sample;
extern int func_0009E1A1(int, int);                     /* sosTIMERInitSystem */
extern int func_0009E281(int);                          /* sosTIMERUnInitSystem */
extern int func_0009E2BB(int, void (*)(void), int *);   /* sosTIMERRegisterEvent */
extern int func_0009E61A(int);                          /* sosTIMERRemoveEvent */
extern int func_0009E8FF(char *, int);                  /* sosDIGIInitSystem */
extern int func_0009E95B(void);                         /* sosDIGIUnInitSystem */
extern int func_0009F4DE(void *, int *);                /* sosDIGIInitDriver */
extern int func_0009F9A7(int, int, int);                /* sosDIGIUnInitDriver */
extern int func_000A2504(int, struct sos_sample *);     /* sosDIGIStartSample */
extern short func_000A2460(int, int);                   /* sosDIGISampleDone */
extern int func_000A2687(int, int);                     /* sosDIGIStopSample */
extern int func_000A1ED5(int, int, int);                /* sosDIGISetSampleVolume */
extern int func_000A20BF(int, int, int);                /* sosDIGISetPanLocation */
extern int func_0009E9C2(char *, int);                  /* sosMIDIInitSystem */
extern int func_0009EC0A(void);                         /* sosMIDIUnInitSystem */
extern int func_0009EC82(void *, int *);                /* sosMIDIInitDriver */
extern int func_0009F253(int, int);                     /* sosMIDIUnInitDriver */
extern int func_0009FEE5(int, void __far *, int);       /* sosMIDISetInsData */
extern int func_000A021C(void *, int *);                /* sosMIDIInitSong */
extern int func_000A0517(int);                          /* sosMIDIUnInitSong */
extern int func_000A27A0(int);                          /* sosMIDIStartSong */
extern int func_000A2857(int);                          /* sosMIDIStopSong */
extern int func_000A2941(int);                          /* sosMIDISongDone */
extern int func_000A1D3C(int);                          /* sosMIDISetMasterVolume */

#ifdef DAGGER_PORT
#pragma clang diagnostic pop
#endif

#endif
