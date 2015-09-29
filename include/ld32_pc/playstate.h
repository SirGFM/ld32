/**
 * @file new-src/playstate.c
 * 
 * The game's playstate
 */
#ifndef __PLAYSTATE_STRUCT__

/** List of particle animations */
enum {
    RED_BULLET,
    ORANGE_BULLET,
    YELLOW_BULLET,
    GREEN_BULLET,
    CYAN_BULLET,
    BLUE_BULLET,
    PURPLE_BULLET,
    MAX_PART_ANIM
};

#define __PLAYSTATE_STRUCT__

#endif /* __PLAYSTATE_STRUCT__ */

#ifndef __PLAYSTATE_H__
#define __PLAYSTATE_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmGroup.h>

#include <ld32_pc/game.h>
#include <ld32_pc/player.h>

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

/**
 * Store the current checkpoint, if the player overlaps against it
 * 
 * @param  [in]pPl   The player
 * @param  [in]pGame The game context
 * @param  [in]pCp   The checkpoint
 * @return           GFMRV_OK, ...
 */
gfmRV ps_plCollideCheckpoint(void *pPlayer, gameCtx *pGame, void *pCp);

/**
 * Retrive the state's particle group
 * 
 * @param  [out]ppGrp The retrieved group
 * @param  [in] pGame The game context
 * @return            GFMRV_OK, ...
 */
gfmRV ps_getParticles(gfmGroup **ppGrp, gameCtx *pGame);

/**
 * Retrieve the player's reference
 * 
 * @param  [out]ppPlayer The retrieved player
 * @param  [in] pGame    The game context
 * @return               GFMRV_OK, ...
 */
gfmRV ps_getPlayer(player **ppPlayer, gameCtx *pGame);

#endif /* __PLAYSTATE_H__ */

