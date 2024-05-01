#ifndef COLLISION_H
#define COLLISION_H


#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>


struct collision_node {
	/** The colliding object. */
	gfmObject *object;
	/** The sprite associated with this object, if any. */
	gfmSprite *sprite;
	/** The generic object associated with this object, if any. */
	void *child;
	/** The colliding object's type. */
	int type;
};


/**
 * collision_handle checks if the two objects currently overlapping in the quadtree
 * are able to interact,
 * forwarding them to the appropriate handler.
 *
 * @param [in] qt: The quadtree.
 * @return 0: Success; Anything else: failure.
 */
int collision_handle(gfmQuadtreeRoot *qt);


/**
 * collision_handleFloor handles colliding a floor with some other object.
 *
 * @param [in] floor: The floor node.
 * @param [in] other: The colliding node.
 * @return 0: Success; Anything else: failure.
 */
int collision_handleFloor(struct collision_node *floor, struct collision_node *other);


/**
 * collision_handleSpikes handles colliding spikes with some other object.
 *
 * @param [in] spikes: The spikes node.
 * @param [in] other: The colliding node.
 * @return 0: Success; Anything else: failure.
 */
int collision_handleSpikes(struct collision_node *spikes, struct collision_node *other);


#endif /* COLLISION_H */
