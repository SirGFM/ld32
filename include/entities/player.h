#ifndef PLAYER_H
#define PLAYER_H


#include <entity.h>


struct player {
	/** The base entity. */
	struct entity base;
};


/**
 * player_new allocates and initializes a new player in the specified position.
 *
 * @param [out] entity: The new player.
 * @param [in] x: The player's horizontal position;
 * @param [in] y: The player's horizontal position;
 * @return 0: Success; Anything else: failure.
 */
int player_new(struct entity **entity, int x, int y);


#endif /* PLAYER_H */
