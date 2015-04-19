/**
 * @file src/player.h
 * 
 * Player module
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/GFraMe_error.h>

#include "camera.h"
#include "sprite.h"

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
 * Collides a player against various sprites
 */
void pl_collideAgainstSprGroup(player *pPl, sprite **pSprs, int sprsLen,
        int isPlFixed, int isSprsFixed);

/**
 * Give another stone to the player
 */
void pl_addStone(player *pPl, sprType type);

/**
 * Returns whether the player is shooting or not (1 on true)
 */
int pl_isShooting(player *pPl);

/**
 * Get all params needed to shot
 */
void pl_getShotParams(int *iniX, int *iniY, int *sX, int *sY, sprType *stones,
        player *pPl);

/**
 * Renders the player
 */
void pl_draw(player *pPl, camera *pCam);

/**
 * Updates the player
 */
void pl_update(player *pPl, int ms);

#endif /* __PLAYER_H__ */

