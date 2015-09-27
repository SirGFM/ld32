/**
 * @file include/ld32_pc/ui.h
 * 
 * Module for rendering the UI; No logic should be kept here!
 */
#ifndef __UI_H__
#define __UI_H__

#include <GFraMe/gfmError.h>

#include <ld32_pc/game.h>

/**
 * Draw the game's UI
 * 
 * @param [in]pGame The game context
 * @return          GFMRV_OK
 */
gfmRV ui_draw(gameCtx *pGame);

#endif /* __UI_H__ */

