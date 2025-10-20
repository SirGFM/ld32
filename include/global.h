#ifndef GLOBAL_H
#define GLOBAL_H


#include <core/types.h>


/** Values saved/restored between sessions. */
struct permanentData {
	/** Bitmask of the collected stones. */
	enum rainbowColor stones;
};


/**
 * global_getPermanent retrieves the global permanents.
 *
 * @return The permanent values for this session.
 */
struct permanentData global_getPermanent();


/**
 * global_setPermanent updates the global permanents.
 *
 * @param [in] data: The permanent values for this session.
 */
void global_setPermanent(struct permanentData data);


/**
 * global_addRedStone update the global permanents
 * with the red stone flag.
 */
void global_addRedStone();


/**
 * global_addOrangeStone update the global permanents
 * with the orange stone flag.
 */
void global_addOrangeStone();


/**
 * global_addYellowStone update the global permanents
 * with the yellow stone flag.
 */
void global_addYellowStone();


/**
 * global_addGreenStone update the global permanents
 * with the green stone flag.
 */
void global_addGreenStone();


/**
 * global_addCyanStone update the global permanents
 * with the cyan stone flag.
 */
void global_addCyanStone();


/**
 * global_addBlueStone update the global permanents
 * with the blue stone flag.
 */
void global_addBlueStone();


/**
 * global_addPurpleStone update the global permanents
 * with the purple stone flag.
 */
void global_addPurpleStone();


#endif /* GLOBAL_H */
