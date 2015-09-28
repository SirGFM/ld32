/**
 * @file src/ui.c
 * 
 * Module for rendering the UI; No logic should be kept here!
 */
#include <GFraMe/gfmAssert.h>
#include <GFraMe/gfmError.h>

#include <ld32_pc/game.h>
#include <ld32_pc/player.h>
#include <ld32_pc/playstate.h>
#include <ld32_pc/ui.h>

/**
 * Draw the game's UI
 * 
 * @param [in]pGame The game context
 * @return          GFMRV_OK
 */
gfmRV ui_draw(gameCtx *pGame) {
    gfmRV rv;
    player *pPlayer;
    
    // Draw the player's 'power meter'
    rv = ps_getPlayer(&pPlayer, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    rv = pl_drawPowerBar(pPlayer, pGame, 0/*x*/,
            GAME_BBUF_HEIGHT - GAME_UI_HEIGHT/*y*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

