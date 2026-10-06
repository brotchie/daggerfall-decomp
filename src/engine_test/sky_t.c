/* sky_t.c: test shims of src/engine/sky.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xsky.h"

void xn_sky_init_stars_r(xn_regs *r)
{
    (void)r;
    xn_sky_init_stars();
}

void xn_sky_draw_stars_r(xn_regs *r)
{
    (void)r;
    xn_sky_draw_stars();
}

void xn_sky_rotate_stars_r(xn_regs *r)
{
    (void)r;
    xn_sky_rotate_stars();
}

void xn_sky_draw_snow_r(xn_regs *r)
{
    (void)r;
    xn_sky_draw_snow();
}

void xn_sky_snow_init_r(xn_regs *r)
{
    (void)r;
    xn_sky_snow_init();
}

void xn_sky_snow_update_speeds_r(xn_regs *r)
{
    (void)r;
    xn_sky_snow_update_speeds();
}

void xn_sky_draw_rain_r(xn_regs *r)
{
    (void)r;
    xn_sky_draw_rain();
}

/* x EAX, y EDX */
void xn_sky_draw_rain_streak_r(xn_regs *r)
{
    xn_sky_draw_rain_streak(r->eax, r->edx);
}

/* dst EAX, n ECX, y_end EDX, colours ESI */
void xn_sky_draw_rain_streak_bottom_clip_r(xn_regs *r)
{
    xn_sky_draw_rain_streak_bottom_clip((u8 *)r->eax, r->ecx, r->edx, (const u8 *)r->esi);
}

/* src EAX, dst EDX, width EBX, rows ECX */
void xn_sky_copy_rows_r(xn_regs *r)
{
    xn_sky_copy_rows((const u8 *)r->eax, (u8 *)r->edx, r->ebx, r->ecx);
}
