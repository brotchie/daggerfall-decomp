/* xscreen.h: helpers the screen group's readable C (gfx, pal, vid, water) needs that belong
   in xngine.h (the coordinator promotes them; see docs/engine/smc/index.md). */
#ifndef XSCREEN_H
#define XSCREEN_H

#include "xngine.h"

/* KEEP (smc/index.md): a self-patched operand the asm keeps in its code. The C computes the
   value in a local and stores the field once, where the asm stores it, for the records. */
#ifndef XN_KEEP
#define XN_KEEP(field, v)   ((field) = (v))
#endif

#endif
