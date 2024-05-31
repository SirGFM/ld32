#include <collision.h>
#include <error.h>


/**
 * _collision_sweepCollision collides two objects,
 * separating them even if they are moving too fast past each other.
 *
 * @param [in] ref: The object used as reference for the sweep collision.
 * @param [in] obj: The other object involved in the collision.
 * @return 0: on collision, 1: otherwise.
 */
int _collision_sweepCollision(gfmObject *ref, gfmObject *obj) {
	gfmRV grv;

	/* Try to separate the objects regularly,
	 * as this should work in most cases. */
	grv = gfmObject_collide(ref, obj);
	if (grv != GFMRV_TRUE) {
		/* If the objects didn't collide regularly,
		 * one of the objects is most likely going too fast past the other. */
		grv = gfmObject_sweepJustOverlaped(ref, obj);
		if (grv == GFMRV_TRUE) {
			gfmObject_sweepCollision(ref, obj);
		}
	}

	return grv != GFMRV_TRUE;
}


/**
 * _collision_sweepJustOverlap overlaps two objects,
 * ensuring that they just overlapped each other
 * even if they are moving too fast.
 *
 * @param [in] ref: The object used as reference for the sweep overlap.
 * @param [in] obj: The other object involved in the overlap check.
 * @return 0: on collision, 1: otherwise.
 */
int _collision_sweepJustOverlap(gfmObject *ref, gfmObject *obj) {
	gfmRV grv;

	/* Try to overlap the objects regularly,
	 * as this should work in most cases. */
	grv = gfmObject_justOverlaped(ref, obj);
	if (grv != GFMRV_TRUE) {
		/* If the objects didn't overlap regularly,
		 * one of the objects is most likely going too fast past the other. */
		grv = gfmObject_sweepJustOverlaped(ref, obj);
	}

	return grv != GFMRV_TRUE;
}


int collision_handleFloor(struct collision_node *floor, struct collision_node *other) {
	gfmCollision dir;
	gfmRV grv = GFMRV_OK;

	/* Ensure that the objects are colliding,
	 * since the extended hitbox may graze each other
	 * without triggering a collision. */
	if (_collision_sweepCollision(floor->object, other->object)) {
		return 0;
	}

	ASSERT_OK(grv = gfmObject_getCurrentCollision(&dir, other->object), __ret);

	/* Handle bonking/stopping on the floor. */
	if (dir & gfmCollision_ver) {
		gfmObject_setVerticalVelocity(other->object, 0);
	}

	/* Handle getting stuck in the corner of platforms
	 * (e.g., moving left out of a platform and immediately pressing right),
	 * moving the entity back on top of the platform. */
	if ((dir & gfmCollision_down) && (dir & gfmCollision_hor)) {
		int x, y;

		ASSERT_OK(grv = gfmObject_getPosition(&x, &y, other->object), __ret);
		if (dir & gfmCollision_left) {
			x--;
		}
		else {
			x++;
		}

		y--;
		ASSERT_OK(grv = gfmObject_setPosition(other->object, x, y), __ret);
	}

__ret:
	return grv;
}


int collision_handleSpikes(struct collision_node *spikes, struct collision_node *other) {
	/* Ensure that the objects are overlapping,
	 * since the extended hitbox may graze each other
	 * without triggering a collision. */
	if (_collision_sweepJustOverlap(spikes->object, other->object)) {
		return 0;
	}

	/* TODO:
	 *   - Directional spikes;
	 *       - Separate  object if hitting on an invalid direction;
	 *   - More lenient hitbox on the spikes (for damaging);
	 *   - Send damage/kill event to other.
	 */

	return 0;
}


int collision_handlePlayerLoader(struct collision_node *player, struct collision_node *loader) {
	/* Ensure that the objects are colliding,
	 * since the extended hitbox may graze each other
	 * without triggering a collision. */
	if (_collision_sweepCollision(player->object, loader->object)) {
		return 0;
	}

	/* TODO: Configure the level to be loaded. */

	return 0;
}


int collision_collideSprite(gfmQuadtreeRoot *qt, gfmSprite *sprite) {
	/** GFraMe return value */
	gfmRV grv = GFMRV_OK;
	int rv = 0;

	rv = gfmQuadtree_collideSprite(qt, sprite);
	ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, __ret);

	/* Continue colliding until the quadtree finishes. */
	while (rv != GFMRV_QUADTREE_DONE) {
		ASSERT_OK(rv = collision_handle(qt), __ret);

		rv = gfmQuadtree_continue(qt);
		ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE, __ret);
	}

	rv = GFMRV_OK;
__ret:
	return grv || rv;
}
