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
 * global_getEphemeral retrieves the global ephemerals.
 *
 * @return The ephemeral values for this session.
 */
struct ephemeralData global_getEphemeral();


#endif /* GLOBAL_H */
