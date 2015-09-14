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
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmSpriteset.h>

#define player       gfmType_reserved_2
#define collideable  gfmType_reserved_3
#define spike        gfmType_reserved_4
#define checkpoint   gfmType_reserved_5
#define powerstone   gfmType_reserved_6

struct stGameCtx {
/** == Game and state contexts ============================================== */
    /** The library's main context */
    gfmCtx *pCtx;
    /** PRNG seed */
    unsigned int seed;
    /** Pointer to the current state's struct */
    void *pState;
    /** Definition of the current state's type */
    stateTypes state;
    /** Whether it's the first frame */
    int firstFrame;
/** == Options stuff ======================================================== */
    /** Maximum number of particles on screen */
    int maxParticless;
/** == Spritesets =========================================================== */
    /** 2x2 spriteset */
    gfmSpriteset *pSset2x2;
    /** 4x4 spriteset */
    gfmSpriteset *pSset4x4;
    /** 8x8 spriteset */
    gfmSpriteset *pSset8x8;
    /** 16x16 spriteset */
    gfmSpriteset *pSset16x16;
/** ========================================================================= */
    /** The game's quadtree */
    gfmQuadtreeRoot *pQt;
/** == Input handles ======================================================== */
    int hndDown;
    int hndLeft;
    int hndRight;
    int hndJump;
    int hndShoot;
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
/** == Audio stuff ========================================================== */
    int sndStep;
    int sndJump;
    int sndDeath;
    int sndBullet;
    int sndPowerup;
    int sndRevive;
    int sndText;
    int sndSong;
};

#endif /* __GAME_PC_H__ */

