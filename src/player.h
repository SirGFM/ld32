/**
 * @file src/player.h
 * 
 * Player module
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/GFraMe_error.h>

#include "camera.h"

/** 'Export' the player structure */
typedef struct stPlayer player;

/**
 * Alloc a new player
 */
int pl_getNew(player **ppPl);

/**
 * Free a player's memory
 */
void pl_free(player **ppPl);

/**
 * Initialize the player and its position
 */
int pl_init(player *pPl, int x, int y);

/**
 * Collides a player against various objects
 */
void pl_collideAgainstGroup(player *pPl, GFraMe_object *pObjs, int objsLen,
        int isPlFixed, int isObjsFixed);

/**
 * Renders the player
 */
void pl_draw(player *pPl, camera *pCam);

/**
 * Updates the player
 */
void pl_update(player *pPl, int ms);

#endif /* __PLAYER_H__ */

