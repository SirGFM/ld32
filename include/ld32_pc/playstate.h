/**
 * @file new-src/playstate.c
 * 
 * The game's playstate
 */
#ifndef __PLAYSTATE_H__
#define __PLAYSTATE_H__

#include <GFraMe/gfmError.h>

#include <ld32_pc/game.h>

/**
 * Initialize the playstate: stores the playstate context, loads the map, etc
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV ps_init(gameCtx *pGame);

/**
 * Updates the playstate a single frame
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_update(gameCtx *pGame);

/**
 * Draw a single frame from the playstate
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_draw(gameCtx *pGame);

/**
 * Clean up all alloc'ed memory
 * 
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_clean(gameCtx *pGame);

/**
 * Retrieve the player's starting position
 * 
 * @param  [in]pX    The horizontal position
 * @param  [in]pY    The vertical position
 * @param  [in]pGame The game context
 * @return           GFMRV_OK, ...
 */
gfmRV ps_getPlayerInitPos(int *pX, int *pY, gameCtx *pGame);

#endif /* __PLAYSTATE_H__ */

