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


/**
 * collision_collideSprite collides the given sprite with the provided quadtree.
 *
 * @param [in] qt: The quadtree.
 * @param [in] sprite: The colliding entity.
 * @return 0: Success; Anything else: failure.
 */
int collision_collideSprite(gfmQuadtreeRoot *qt, gfmSprite *sprite);


/**
 * collision_collideGroup collides the given group with the provided quadtree.
 *
 * @param [in] qt: The quadtree.
 * @param [in] group: The colliding group.
 * @return 0: Success; Anything else: failure.
 */
int collision_collideGroup(gfmQuadtreeRoot *qt, gfmGroup *group);


/**
 * collision_handlePlayerLoader handles colliding a player with a loader,
 * queueing a scene transition.
 *
 * @param [in] player: The player node.
 * @param [in] loader: The loader node.
 * @return 0: Success; Anything else: failure.
 */
int collision_handlePlayerLoader(struct collision_node *player, struct collision_node *loader);


/**
 * collision_explodeRainbow handles colliding a solid (e.g., the floor)
 * with a rainbow bullet.
 *
 * @param [in] solid: The solid (e.g., floor) node.
 * @param [in] bullet: The rainbow particle node.
 * @return 0: Success; Anything else: failure.
 */
int collision_explodeRainbow(struct collision_node *solid, struct collision_node *bullet);


/**
 * collision_yokuBlock handles colliding an object with a yoku block.
 *
 * @param [in] obj: The obj node.
 * @param [in] loader: The yoku block node.
 * @return 0: Success; Anything else: failure.
 */
int collision_yokuBlock(struct collision_node *obj, struct collision_node *block);


/**
 * collision_getGem handles colliding a player with a gem.
 *
 * @param [in] obj: The obj node.
 * @param [in] loader: The yoku block node.
 * @return 0: Success; Anything else: failure.
 */
int collision_getGem(struct collision_node *player, struct collision_node *gem);


#endif /* COLLISION_H */
