/**
 * @file src/global.c
 */
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_spriteset.h>
#include <GFraMe/GFraMe_texture.h>

#include <stdio.h>
#include <stdlib.h>

#include "global.h"

int gl_running = 0;
static int is_init = 0;

#define DECLARE_SSET(W, H) \
  GFraMe_spriteset *gl_sset##W##x##H; \
  static GFraMe_spriteset _glSset##W##x##H

#define DECLARE_AUDIO(AUD) \
  static GFraMe_audio  _glAud_##AUD; \
  GFraMe_audio * gl_aud_##AUD

static GFraMe_texture gl_tex;
DECLARE_SSET(2, 2);
DECLARE_SSET(4, 4);
DECLARE_SSET(8, 8);
DECLARE_SSET(16, 16);

GFraMe_ret gl_init() {
    GFraMe_ret rv;
    unsigned char *data = NULL;

    rv = GFraMe_assets_buffer_image(TEX, TEXW, TEXH, (char**)&data);
    ASSERT_NR(rv == GFraMe_ret_ok);

    GFraMe_texture_init(&gl_tex);
    rv = GFraMe_texture_load(&gl_tex, TEXW, TEXH, data);
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    /**
     * Initialize the spriteset of a given dimensions
     */
    #define INIT_SSET(W, H) \
      gl_sset##W##x##H = &_glSset##W##x##H; \
      GFraMe_spriteset_init(gl_sset##W##x##H, &gl_tex, W, H)
    
    INIT_SSET(2, 2);
    INIT_SSET(4, 4);
    INIT_SSET(8, 8);
    INIT_SSET(16, 16);
    
    #define INIT_AUDIO(AUD, FILEN) \
      rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 0, 0, 1); \
      GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
      gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG(AUD, FILEN) \
      rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 1, 0, 1); \
      GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
      gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG_WINTRO(AUD, FILEN, LOOPPOS) \
      rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 1, LOOPPOS, 1); \
      GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
      gl_aud_##AUD = &_glAud_##AUD
    #define INIT_SONG_NOLOOP(AUD, FILEN) \
      rv = GFraMe_audio_init(&_glAud_##AUD, FILEN, 0, 0, 1); \
      GFraMe_assertRet(rv == GFraMe_ret_ok, "Loading audio "FILEN" failed", __ret); \
      gl_aud_##AUD = &_glAud_##AUD
    
    gl_running = 1;
    is_init = 1;
    rv = GFraMe_ret_ok;
__ret:
    if (data)
        free(data);
    return rv;
}

void gl_clean() {
    if (is_init) {
        GFraMe_texture_clear(&gl_tex);
    }
    is_init = 0;
}

