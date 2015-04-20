/**
 * @file src/main.c
 * 
 * The game's entry point
 */
#include <GFraMe/GFraMe.h>
#include <GFraMe/GFraMe_audio_player.h>
#include <GFraMe/GFraMe_controller.h>
#include <GFraMe/GFraMe_screen.h>

#include "global.h"
#include "playstate.h"

int main(int argc, char *argv[]) {
    GFraMe_ret rv;
    GFraMe_wndext ext;

    ext.atlas = TEX;
    ext.atlasWidth = TEXW;
    ext.atlasHeight = TEXH;
    ext.flags = GFraMe_wndext_none;

    rv = GFraMe_init
        (
         SCRW,
         SCRH,
         WNDW,
         WNDH,
         "com.gfmgamecorner",
         TITLE,
         GFraMe_window_none,
         &ext,
         FPS,
         LOGTOFILE,
         0
        );
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    rv = GFraMe_audio_player_init();
    ASSERT_NR(rv == GFraMe_ret_ok);

    rv = gl_init();
    ASSERT_NR(rv == GFraMe_ret_ok);
    
    GFraMe_controller_init(1/*autoConnect*/);

    while (gl_running) {
        playstate();
    }

__ret:
    GFraMe_audio_player_pause();
    GFraMe_audio_player_clear();
    
    GFraMe_controller_close();
    gl_clean();
    GFraMe_quit();
    return rv;
}


