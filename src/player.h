/**
 * @file src/player.h
 * 
 * Player module
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <GFraMe/GFraMe_error.h>

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

#endif /* __PLAYER_H__ */

