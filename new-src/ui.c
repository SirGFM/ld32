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
#include <ld32_pc/stone.h>
#include <ld32_pc/ui.h>

/**
 * Draw the game's UI
 * 
 * @param [in]pGame The game context
 * @return          GFMRV_OK
 */
gfmRV ui_draw(gameCtx *pGame) {
    gfmRV rv;
    int tile, type, x, y;
    player *pPlayer;
    
    // Draw the player's 'power meter'
    rv = ps_getPlayer(&pPlayer, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    rv = pl_drawPowerBar(pPlayer, pGame, 0/*x*/,
            GAME_BBUF_HEIGHT - GAME_UI_HEIGHT/*y*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    // Get the position (already on the border) and type of the next stone
    rv = ps_getNextStone(&x, &y, &type, pGame);
    ASSERT(rv == GFMRV_OK, rv);
    if (x < 8) {
        x = 0;
    }
    else if (x + 8 >= GAME_BBUF_WIDTH) {
        x = GAME_BBUF_WIDTH - 8;
    }
    else {
        x -= 4;
    }
    if (y < 8) {
        y = 0;
    }
    else if (y + 8 >= GAME_BBUF_HEIGHT - GAME_UI_HEIGHT) {
        y = GAME_BBUF_HEIGHT - GAME_UI_HEIGHT - 8;
    }
    else {
        y -= 4;
    }
    
    switch (type) {
        case RED_STONE:    { tile = 369; } break;
        case ORANGE_STONE: { tile = 370; } break;
        case YELLOW_STONE: { tile = 371; } break;
        case GREEN_STONE:  { tile = 372; } break;
        case CYAN_STONE:   { tile = 373; } break;
        case BLUE_STONE:   { tile = 374; } break;
        case PURPLE_STONE: { tile = 375; } break;
        default:           { tile = -1; }
    }
    rv = gfm_drawTile(pGame->pCtx, pGame->pSset8x8, x, y, tile, 0/*flip*/);
    ASSERT(rv == GFMRV_OK, rv);
    
    rv = GFMRV_OK;
__ret:
    return rv;
}

