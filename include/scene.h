#ifndef SCENE_FORWARD_H
#define SCENE_FORWARD_H
struct scene;
#endif /* SCENE_FORWARD_H */

#if !defined(SCENE_H) && !defined(FORWARD_ONLY)
#define SCENE_H


#include <core/map.h>
#include <entity.h>

#include <GFraMe/gfmQuadtree.h>


struct scene {
	/**
	 * map stores the scene's tilemaps.
	 * This object should be managed elsewhere,
	 * and isn't freed by the scene.
	 */
	struct map *map;
	/**
	 * staticCollider allows colliding entities against the tilemaps.
	 * This collider is populated only once, when the scene is loaded.
	 */
	gfmQuadtreeRoot *staticCollider;
	/**
	 * dynamicCollider manages colliding objects against each other.
	 * This collider is reset and populated every frame.
	 */
	gfmQuadtreeRoot *dynamicCollider;
	/**
	 * entities lists every entity loaded by the map
	 * (for example: the player, gems, etc).
	 *
	 * Since each entity may have a different sub-structure,
	 * they must be allocated independently.
	 *
	 * The length of this list matches the total of objects in map
	 * (i.e., sum(map->objects[i].count, for i in 0..map->numObjectLists).
	 * For simplicity, this value is pre-calculated and stored.
	 */
	struct entity **entities;
	/** How many entities were loaded into this scene. */
	int numEntities;
	/** The scene's width in pixels. */
	int width;
	/** The scene's height in pixels. */
	int height;
};


/**
 * scene_loadScene loads a scene from a pre-loaded map,
 * instantiating objects as needed.
 *
 * @param [in] scene: The struct where the scene is loaded.
 * @param [in] map: The pre-loaded used to load the scene.
 * @return 0: Success; Anything else: failure.
 */
int scene_loadScene(struct scene *scene, struct map *map);


/**
 * scene_update updates the entire scene,
 * running physics simulation, collision and custom per-object update logic.
 *
 * @param [in] scene: The scene being updated.
 * @return 0: Success; Anything else: failure.
 */
int scene_update(struct scene *scene);


/**
 * scene_draw renders the provided scene
 * offsetting it by the amount of pixels provided
 * from the top-left corner of the window.
 *
 * @param [in] scene: The scene being rendered.
 * @param [in] x: The horizontal offset.
 * @param [in] y: The vertical offset.
 * @return 0: Success; Anything else: failure.
 */
int scene_draw(struct scene *scene, int x, int y);


/**
 * scene_free releases every resource associated with the provided scene.
 *
 * @param [in] scene: The scene being unloaded.
 * @return 0: Success; Anything else: failure.
 */
int scene_free(struct scene *scene);


#endif /* !defined(SCENE_H) && !defined(FORWARD_ONLY) */
