/**
 * @file src/global.h
 */
#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>

#define SCRW    320
#define SCRH    240
#define WNDW    640
#define WNDH    480
#define TITLE   "Lame"
#define TEX     "atlas"
#define TEXW    128
#define TEXH    128
#define FPS     60
#define UPS     60  // updates per second
#define DPS     60  // draws per second
#define LOGTOFILE   1

#define ASSERT(stmt) \
  do { \
    if (!(stmt)) \
      goto __ret; \
  } while (0)

extern int gl_running;

GFraMe_ret gl_init();
void gl_clean();

#endif

