/**
 * @file src/audio.c
 * 
 * audio module
 */
#include <GFraMe/GFraMe_audio_player.h>

#include "audio.h"
#include "global.h"

static const double sfx_vol = 0.6;
static const double song_vol = 0.8;

void aud_playSong() {
#ifndef MUTED
    GFraMe_audio_player_play_bgm(gl_aud_song1, song_vol);
#endif
}

void aud_playText() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_text, sfx_vol * 0.5);
#endif
}

void aud_playPlStep() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_step, sfx_vol * 0.425);
#endif
}

void aud_playPlFall() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_step, sfx_vol);
#endif
}

void aud_playBlBullet() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_bullet, sfx_vol*0.15);
#endif
}

void aud_playPlDeath() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_death, sfx_vol);
#endif
}

void aud_playPlJump() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_jump, sfx_vol);
#endif
}

void aud_playPlGetStone() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_powerup, sfx_vol);
#endif
}

void aud_playPlRevive() {
#ifndef MUTED
    GFraMe_audio_player_push(gl_aud_revive, sfx_vol);
#endif
}

