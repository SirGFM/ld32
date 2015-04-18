/**
 * @file src/player.h
 * 
 * Player module
 */
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_hitbox.h>
#include <GFraMe/GFraMe_object.h>
#include <GFraMe/GFraMe_sprite.h>
#include <GFraMe/GFraMe_spriteset.h>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "player.h"

/** 'Export' the player structure */
struct stPlayer {
    /** The player's sprite */
    GFraMe_sprite *pSpr;
};

/**
 * Alloc a new player
 */
int pl_getNew(player **ppPl) {
    int rv;
    
    // Check params
    ASSERT(ppPl, 1);
    ASSERT(!(*ppPl), 1);
    
    // Alloc the player
    *ppPl = (player*)malloc(sizeof(player));
    ASSERT(*ppPl, 1);
    
    // Clean every variable
    (*ppPl)->pSpr = 0;
    
    // Alloc every needed variable
    (*ppPl)->pSpr = (GFraMe_sprite*)malloc(sizeof(GFraMe_sprite));
    ASSERT((*ppPl)->pSpr, 1);
    memset((*ppPl)->pSpr, 0x0, sizeof(GFraMe_sprite));
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Free a player's memory
 */
void pl_free(player **ppPl) {
    // Check params
    ASSERT_NR(ppPl);
    ASSERT_NR(*ppPl);
    
    if ((*ppPl)->pSpr) {
        free((*ppPl)->pSpr);
    }
    
    // Free the player
    free(*ppPl);
    *ppPl = 0;
    
__ret:
    return;
}

