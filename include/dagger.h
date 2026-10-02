/* Shared declarations for FALL.EXE. Names are addresses until a better name is known:
 * D_XXXXXXXX for data, func_XXXXXXXX for code. Each global has one declaration here, so every
 * unit agrees on its type. */
#ifndef DAGGER_H
#define DAGGER_H

/* data */
extern int D_0018467C;
extern int D_00190BE4;
extern int D_00190BE8;
extern unsigned char D_00190C78;
extern int D_00190DF4;
extern char *D_001959A8;
extern char *D_00195AA4;
extern char *D_00195AC4;
extern int D_00195AF4;
extern int D_00195B84;
extern char *D_00195BE0;
extern char D_00195E7A;
extern int D_0019672C;
extern int D_0019972C;
extern short D_00199788[];

/* code */
extern void func_00013981();
extern int func_00019323(int, int);
extern void func_000193DD(int);
extern unsigned char func_0002010F(int, int);
extern int func_000309E8(char *, short);
extern void func_0005E450(unsigned short, char *);
extern int func_0008B572(unsigned char, unsigned char);
extern char *func_0008DCE3(char *, int, int);
extern void func_0008E3F7(int, void (*)());
extern char *func_0008E925(char *, int);
extern void func_0008ECBD(char *, int);
extern int func_0009DC25(void);
extern void func_0009DC49(int);
extern int func_000C7FD9(int, int, int, int);

#endif
