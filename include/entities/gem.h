#ifndef GEM_H
#define GEM_H


#include <core/map.h>
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


#endif /* GEM_H */
