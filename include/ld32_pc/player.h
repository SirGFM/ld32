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

#endif /* __PLAYER_H__ */

