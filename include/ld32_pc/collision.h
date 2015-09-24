/**
 * @file include/ld32_pc/collision.h
 * 
 * Check collisions and handle 'em accordingly
 */
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <GFraMe/gfmSprite.h>
#include <ld32_pc/game.h>

/**
 * Collide a sprite against the world
 * 
 * @param  [in]pGame The game context
 * @param  [in]pSpr  The sprite
 * @return           GFMRV_OK, ...
 */
gfmRV collideSprite(gameCtx *pGame, gfmSprite *pSpr);

#endif /* __COLLISION_H__ */

