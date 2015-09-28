/**
 * @file include/ld32_pc/player.h
 * 
 * The player
 */
#ifndef __PLAYER_ST__
#define __PLAYER_ST__

/** Export the player type */
typedef struct stPlayer player;

#endif /* __PLAYER_ST__ */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld32_pc/game.h>

/**
 * Initialize a player
 * 
 * @param  [out]ppCtx   The player
 * @param  [in] pGame   The game context (to retrieve the texture and spriteset)
 * @param  [in] pParser The parser context (to retrieve the player's attributes)
 * @return              GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV pl_init(player **ppCtx, gameCtx *pGame, gfmParser *pParser);

/**
 * Free all memory used by the player
 * 
 * @param  [in|out]ppCtx The player
 */
void pl_clean(player **ppCtx);

/**
 * Collide the player against the wall/floor
 * 
 * @param  [in]pCtx  The player
 * @param  [in]pGame The game context
 * @param  [in]pWall The wall
 * @return           GFMRV_OK, ...
 */
gfmRV pl_collideWall(player *pCtx, gameCtx *pGame, gfmObject *pWall);

/**
 * Collide the player against a spike
 * 
 * @param  [in]pCtx   The player
 * @param  [in]pGame  The game context
 * @param  [in]pSpike The spike
 * @return            GFMRV_OK, ...
 */
gfmRV pl_collideSpike(player *pCtx, gameCtx *pGame, gfmObject *pSpike);

/**
 * Update the player
 * 
 * @param  [in]pCtx The player
 * @return          GFMRV_OK, ...
 */
gfmRV pl_update(player *pPlayer, gameCtx *pGame);

/**
 * Render the player
 * 
 * @param  [in]pCtx The player
 * @return          GFMRV_OK, ...
 */
gfmRV pl_draw(player *pPlayer, gameCtx *pGame);

/**
 * Draw the player's power bar
 * 
 * @param  [in]pCtx   The player
 * @param  [in]pGame  The game context
 * @param  [in]x      The horizontal position
 * @param  [in]y      The vertical position
 * @return            GFMRV_OK, ...
 */
gfmRV pl_drawPowerBar(player *pPlayer, gameCtx *pGame, int x, int y);

#endif /* __PLAYER_H__ */

