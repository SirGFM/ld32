#ifndef GEM_H
#define GEM_H


#include <core/map.h>
#include <core/types.h>
#include <entity.h>


struct gem {
	/** The base entity. */
	struct entity base;
	/** Whether this gem is still active. */
	int active;
	/** How long into the wobble animation the gem is. */
	int timerMs;
};


/**
 * gem_new allocates and initializes a new gem.
 *
 * @param [out] entity: The new gem.
 * @param [in] data: The parsed data for the gem.
 * @return 0: Success; Anything else: failure.
 */
int gem_new(struct entity **entity, struct mapObject *data);


/**
 * gem_isActive checks if the gem is still active.
 *
 * @param [in] entity: The gem.
 * @return 1: If the block is active; 0: Otherwise.
 */
int gem_isActive(struct entity *entity);


/**
 * gem_get obtains the gem.
 *
 * @param [in] entity: The gem.
 * @return 0: Success; Anything else: failure.
 */
int gem_get(struct entity *entity);


/**
 * gem_getBaseAnimation retrieves a gem-agnostic animation data,
 * using the offset from the gem's first sprite.
 *
 * @param [out] data: The animation data.
 * @param [out] len: The number of entries in the data.
 */
void gem_getBaseAnimation(int **data, int *len);


/**
 * gem_getFirstFrame retrieves the sprite for a gem's first animation frame.
 *
 * If an invalid color is provided, gray is returned.
 *
 * @param [out] sprite: The sprite's index.
 * @param [in] color: A combination of colors.
 */
void gem_getFirstFrame(int *sprite, enum rainbowColor color);


#endif /* GEM_H */
