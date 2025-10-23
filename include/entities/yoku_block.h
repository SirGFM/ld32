#ifndef YOKU_BLOCK_H
#define YOKU_BLOCK_H


#include <core/map.h>
#include <entity.h>


struct yokuBlock {
	/** The base entity. */
	struct entity base;
	/** ID of the block, so it's controller may calculate the block's timing. */
	int id;
	/** ID of the block controller. */
	int parentID;
	/** How many steps this block stays active for. */
	int durationSteps;
	/** Decreasing timer that blocks the block's timer. */
	int preSleepMs;
	/** How long, in milliseconds, the block should stay active. */
	int activeMs;
	/** How long, in milliseconds, until the block becomes active again. */
	int sleepMs;
	/** The block's timer in the current cycle,
	 * resetting every activeMs+sleepMs. */
	int timer;
};


/**
 * yokuBlock_new allocates and initializes a new yoku block.
 *
 * @param [out] entity: The new yoku block.
 * @param [in] data: The parsed data for the yoku block.
 * @return 0: Success; Anything else: failure.
 */
int yokuBlock_new(struct entity **entity, struct mapObject *data);


/**
 * yokuBlock_setTiming configure's the blocks timing.
 *
 * The cycle duration is rounded down to ensure
 * it's a multiple of the number of steps.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @param [in] numSteps: How many steps are in a cycle.
 * @param [in] cycleMs: How long a cycle takes to repeat, in milliseconds.
 */
void yokuBlock_setTiming(struct entity *entity, int numSteps, int cycleMs);


/**
 * yokuBlock_setTiming configure's the blocks timing.
 *
 * The cycle duration is rounded down to ensure
 * it's a multiple of the number of steps.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @return 1: If the block is active; 0: Otherwise.
 */
int yokuBlock_isActive(struct entity *entity);


#endif /* YOKU_BLOCK_H */
