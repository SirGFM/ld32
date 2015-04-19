/**
 * @file src/ui.c
 * 
 * Module for rendering ui (only renders laser bar)
 */
#include <GFraMe/GFraMe_spriteset.h>

#include "global.h"
#include "player.h"
#include "sprite.h"
#include "ui.h"

void ui_draw(player *pPl) {
    double laserDur;
    int num, x, y, curTile;
    sprType stones, tmp;
    
    pl_getShotInfo(&laserDur, &stones, pPl);
    
    // Count how many stones the player has
    tmp = stones;
    num = 0;
    while (tmp > 0) {
        if (tmp & 1)
            num++;
        tmp >>= 1;
    }
    laserDur *= num;
    
    x = 8;
    y = 8;
    tmp = SPR_RED_STONE;
    curTile = 261;
    while (tmp < 0x100) {
        int tile;
        
        if (tmp & stones) {
            if (laserDur > 1) {
                laserDur -= 1.0;
                tile = curTile;
            }
            else if (laserDur > 0) {
                int val;
                
                // TODO make it blink faster when it's full
                //val = (int)(laserDur * 10.0);
                //if (val > 5)
                    tile = curTile;
                //else
                //    tile = 260;
                
                laserDur = 0;
            }
            else
                tile = 260;
        }
        else {
            tile = 260;
        }
        
        // Draw the current tile
        GFraMe_spriteset_draw(gl_sset8x8, tile, x, y, 0/*flipped*/);
        
        // Got to the next stone
        tmp <<= 1;
        x += 8;
        curTile++;
    }
}

