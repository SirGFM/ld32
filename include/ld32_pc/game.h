/**
 * @file include/ld32_pc/game.h
 * 
 * Game's basic struct; Keep track of everything used by different objects (like
 * the game's context, textures, sounds, the quadtree, etc...)
 */
#ifndef __GAME_PC_H__
#define __GAME_PC_H__

#include <GFraMe/gframe.h>
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGenericArray.h>
#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmTypes.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>
#include <GFraMe/core/gfmAudio_bkend.h>

#define tPlayer       gfmType_reserved_2
#define tCollideable  gfmType_reserved_3
#define tSpike        gfmType_reserved_4
#define tCheckpoint   gfmType_reserved_5
#define tPowerstone   gfmType_reserved_6

#define GAME_BBUF_WIDTH     320
#define GAME_BBUF_HEIGHT    240
#define GAME_UI_HEIGHT      42
#define GAME_GRAV           500

struct stGameCtx {
/** == Game and state contexts ============================================== */
    /** The library's main context */
    gfmCtx *pCtx;
    /** PRNG seed */
    unsigned int seed;
    /** Pointer to the current state's struct */
    void *pState;
    /** Whether it's the first frame */
    int firstFrame;
/** == Options stuff ======================================================== */
    /** Maximum number of particles on screen */
    int maxParticles;
    /** How slowly the deadzone moves (higher means slower) */
    int deadzoneSpeed;
/** == Spritesets =========================================================== */
    /** 4x4 spriteset */
    gfmSpriteset *pSset4x4;
    /** 8x8 spriteset */
    gfmSpriteset *pSset8x8;
    /** 16x16 spriteset */
    gfmSpriteset *pSset16x16;
/** == Attributes required by the game ====================================== */
    /** The game's quadtree */
    gfmQuadtreeRoot *pQt;
    /** Whether the camera is in manual mode */
    int isCameraManual;
/** == Input handles ======================================================== */
    int hndLeft;
    int hndRight;
    int hndJump;
    int hndShoot;
    int hndFullscreen;
    int hndMute;
    int hndVolUp;
    int hndVolDown;
    int hndQuit;
/** == Input states ========================================================= */
    gfmInputState stDown;
    int nDown;
    gfmInputState stLeft;
    int nLeft;
    gfmInputState stRight;
    int nRight;
    gfmInputState stJump;
    int nJump;
    gfmInputState stShoot;
    int nShoot;
    gfmInputState stFullscreen;
    int nFullscreen;
    gfmInputState stMute;
    int nMute;
    gfmInputState stVolUp;
    int nVolUp;
    gfmInputState stVolDown;
    int nVolDown;
    gfmInputState stQuit;
    int nQuit;
/** == Audio stuff ========================================================== */
    int sndStep;
    int sndJump;
    int sndDeath;
    int sndBullet;
    int sndPowerup;
    int sndRevive;
    int sndText;
    int sndSong;
    /** Song reference; since it loops and can have its volume modified */
    gfmAudioHandle *pSong;
};
typedef struct stGameCtx gameCtx;

#endif /* __GAME_PC_H__ */

