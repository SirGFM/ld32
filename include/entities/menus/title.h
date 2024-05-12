#ifndef TITLE_H
#define TITLE_H


#include <core/map.h>
#include <entity.h>


/** How many letters should trail behind a rainbow letter. */
#define TITLE_NUM_TRAIL 3


struct title {
	/** The base entity. */
	struct entity base;
	/**
	 * Whether this is a rainbow letter,
	 * enabling the wave and trail effects.
	 */
	int isRainbow;
	/** The letter's original horizontal position. */
	int ox;
	/** The letter's original vertical position. */
	int oy;
	/** Timer used by the wave effect. */
	int timerMs;
	/** Timer used by the trail effect. */
	int trailTimerMs[TITLE_NUM_TRAIL];
};


/**
 * title_new allocates and initializes a new title letter/tile in the specified position,
 * and with the specified mode.
 *
 * @param [out] entity: The new title.
 * @param [in] data: The title's configuration.
 * @return 0: Success; Anything else: failure.
 */
int title_new(struct entity **entity, struct mapObject *data);


#endif /* TITLE_H */
