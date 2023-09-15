/**
 * @file include/ld32_pc/collision.h
 * 
 * Check collisions and handle 'em accordingly
 */
#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <GFraMe/gfmGroup.h>
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

/**
 * Collide a group against the world
 * 
 * @param  [in]pGame The game context
 * @param  [in]pGrp  The group
 * @return           GFMRV_OK, ...
 */
gfmRV collideGroup(gameCtx *pGame, gfmGroup *pGrp);

#endif /* __COLLISION_H__ */

