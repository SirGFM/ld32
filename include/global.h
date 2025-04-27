#ifndef GLOBAL_H
#define GLOBAL_H


enum stone {
	  RED_STONE    = 0x01
	, ORANGE_STONE = 0x02
	, YELLOW_STONE = 0x04
	, GREEN_STONE  = 0x08
	, CYAN_STONE   = 0x10
	, BLUE_STONE   = 0x20
	, PURPLE_STONE = 0x40
};


/** Values saved/restored between sessions. */
struct permanentData {
	/** Bitmask of the collected stones. */
	enum stone stones;
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
