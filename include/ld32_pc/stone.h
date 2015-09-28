/**
 * @file include/ld32_pc/stone.h
 * 
 * Power stones that gives the player a new color to shoot
 */
#ifndef __STONE_STRUCT__
#define __STONE_STRUCT__

/** Export the stone type */
typedef struct stStone stone;

enum {
    RED_STONE    = 0x0001,
    ORANGE_STONE = 0x0002,
    YELLOW_STONE = 0x0004,
    GREEN_STONE  = 0x0008,
    CYAN_STONE   = 0x0010,
    BLUE_STONE   = 0x0020,
    PURPLE_STONE = 0x0040
};

#endif /* __STONE_STRUCT__ */

#ifndef __STONE_H__
#define __STONE_H__

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>

#include <ld32_pc/game.h>

/**
 * Initialize a stone
 * 
 * @param  [out]ppCtx   The stone
 * @param  [in] pGame   The game context (to retrieve the texture and spriteset)
 * @param  [in] pParser The parser context (to retrieve the player's attributes)
 * @return              GFMRV_OK, GFMRV_ALLOC_FAILED, ...
 */
gfmRV st_init(stone **ppCtx, gameCtx *pGame, gfmParser *pParser);

/**
 * Free all memory used by the stone
 * 
 * @param  [in|out]ppCtx The stone
 */
void st_clean(stone **ppCtx);

/**
 * Update the stone
 * 
 * @param  [in]pCtx The stone
 * @return          GFMRV_OK, ...
 */
gfmRV st_update(stone *pStone, gameCtx *pGame);

/**
 * Render the stone
 * 
 * @param  [in]pCtx The stone
 * @return          GFMRV_OK, ...
 */
gfmRV st_draw(stone *pStone, gameCtx *pGame);

#endif /* __STONE_H__ */

