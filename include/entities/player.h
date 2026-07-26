#ifndef PLAYER_H
#define PLAYER_H


#include <core/types.h>
#include <entity.h>

#include <stdint.h>


struct player {
	/** The base entity. */
	struct entity base;
	/** How many rainbow stones the player has collected. */
	int numStones;
	/** For how long the player may fly without touching the floor,
	 * in milliseconds. */
	int maxFlight;
	/** For how long the player has flown without touching the floor,
	 * in milliseconds. */
	int curFlight;
	/** How long has the player been on the ground after maxing out the flying time,
	 * so their energy may be recharged. */
	int flightRecharge;
	/** For how long the selector menu has been open, if at all,
	 * controlling its opening/closing animation. */
	int selectorTimer;
	/** Selected color, if any. */
	enum rainbowColor selectedColor;
	/** Currently highlighted color in the select menu. */
	enum rainbowColor cursorColor;
	/** Whether the player is currently shooting. */
	uint8_t isShooting;
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


/**
 * player_gemGet adjusts the player after obtaining a gem.
 *
 * @param [in] entity: The player.
 * @return 0: Success; Anything else: failure.
 */
int player_gemGet(struct entity *entity);


#endif /* PLAYER_H */
