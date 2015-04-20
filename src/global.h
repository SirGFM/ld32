/**
 * @file src/global.h
 */
#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <GFraMe/GFraMe_audio.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>

#define SCRW    320
#define SCRH    240
#define WNDW    640
#define WNDH    480
#define TITLE   "KITTEN (An Unconventional Weapon)"
#define TEX     "atlas"
#define TEXW    256
#define TEXH    256
#define FPS     60
#define UPS     60  // updates per second
#define DPS     60  // draws per second
#define LOGTOFILE   0

#define TXT_CHAR_DELAY 50
#define TXT_COMPLETE_DELAY 1250
#define TXT_MAX_LINES 3
#define PL_X 2*8
#define PL_Y 54*8
#define PL_TARGET_DIST 24

#define PI 3.1415926
#define GRAV    500
#define PL_VX   50
#define PL_VY   150
#define PL_BUL_SPEED 150
#define PL_BUL_COOLDOWN 80
#define PL_LASER_COOLDOWN 250
#define PL_BUL_DEC 10
#define PL_LASER_INC 80
#define PL_BUL_DANG 1.0
#define CAM_DEADZONE_TIME 2000.0
#define CAM_MAX_RATIO 0.6
#define CAM_MIN_RATIO 0.2
#define RESPAWN_TIME 1500

#define ASSERT(stmt, retVal) \
  do { \
    if (!(stmt)) { \
      rv = retVal; \
      goto __ret; \
    } \
  } while (0)

#define ASSERT_NR(stmt) \
  do { \
    if (!(stmt)) \
      goto __ret; \
  } while (0)

extern int gl_running;

extern GFraMe_spriteset *gl_sset2x2;
extern GFraMe_spriteset *gl_sset4x4;
extern GFraMe_spriteset *gl_sset8x8;
extern GFraMe_spriteset *gl_sset16x16;

extern GFraMe_audio *gl_aud_step;
extern GFraMe_audio *gl_aud_jump;
extern GFraMe_audio *gl_aud_death;
extern GFraMe_audio *gl_aud_bullet;
extern GFraMe_audio *gl_aud_powerup;
extern GFraMe_audio *gl_aud_revive;
extern GFraMe_audio *gl_aud_text;

GFraMe_ret gl_init();
void gl_clean();

#endif

