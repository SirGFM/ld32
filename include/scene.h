#ifndef SCENE_FORWARD_H
#define SCENE_FORWARD_H
struct scene;
#endif /* SCENE_FORWARD_H */

#if !defined(SCENE_H) && !defined(FORWARD_ONLY)
#define SCENE_H


#include <core/map.h>
#include <entity.h>

#include <GFraMe/gfmQuadtree.h>


/**
 * The relative position of a scene in relation to another.
 *
 * SCENE_RESET is a special value that causes the scene to reset back to the origin.
 */
enum scene_relativePosition {
	SCENE_RESET = 0
	, SCENE_LEFT_OF
	, SCENE_RIGHT_OF
	, SCENE_ABOVE
	, SCENE_BELLOW
};


struct scene {
	/**
	 * The loader that triggered a screen transition
	 * and is being used in the screen transition.
	 */
	struct entity *curLoader;
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
	/**
	 * Whether the scene was loaded from a file,
	 * instead of being loaded from a previously loaded map.
	 */
	int fromFile;
	/** The time, in milliseconds, elapsed since the last frame. */
	int elapsedMs;
	/** Whether the collision's bounding boxes should be drawn. */
	int drawCollisions;
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
 * scene_loadSceneFromFile a scene from a file.
 *
 * @param [in] scene: The struct where the scene is loaded.
 * @param [in] dir: The directory, within assets, with the scene.
 * @param [in] len: Length of dir.
 * @return 0: Success; Anything else: failure.
 */
int scene_loadSceneFromFile(struct scene *scene, char *dir, int len);


/**
 * scene_update updates the entire scene,
 * running physics simulation, collision and custom per-object update logic.
 *
 * @param [in] scene: The scene being updated.
 * @return 0: Success; Anything else: failure.
 */
int scene_update(struct scene *scene);


/**
 * scene_draw renders the provided scene.
 *
 * @param [in] scene: The scene being rendered.
 * @return 0: Success; Anything else: failure.
 */
int scene_draw(struct scene *scene);


/**
 * scene_setRelativePosition moves self so its on pos relative to other.
 *
 * In addition to moving self based on other's dimension,
 * doorOffset is used to move self further,
 * ensuring the door/scene transitions are properly connected.
 *
 * After the scene is moved,
 * the camera must be controlled independently
 * (changing the deadzone as need so it may scroll over this other scene)
 * to properly scroll from one room to another.
 *
 * @param [in] self: The scene being moved.
 * @param [in] other: The scene relative to which to move.
 * @param [in] pos: The new relative position of self in relatio to other.
 * @param [in] doorOffsetX: The horizontal door distance, in tiles.
 * @param [in] doorOffsetY: The vertical door distance, in tiles.
 * @return 0: Success; Anything else: failure.
 */
int scene_setRelativePosition(
	struct scene *self
	, struct scene *other
	, enum scene_relativePosition pos
	, int doorOffsetX
	, int doorOffsetY
);


/**
 * scene_getCameraTransitionPosition calculates the position of the camera
 * after a scene transition from other to self.
 *
 * @param [out] x: The camera's target horizontal position.
 * @param [out] y: The camera's target vertical position.
 * @param [in] self: The scene being moved.
 * @param [in] other: The scene relative to which to move.
 * @param [in] doorID: Unique value that identifies the same door in two different scenes.
 * @return 0: Success; Anything else: failure.
 */
int scene_getCameraTransitionPosition(
	int *x
	, int *y
	, struct scene *self
	, struct scene *other
	, int doorID
);


/**
 * scene_free releases every resource associated with the provided scene.
 *
 * @param [in] scene: The scene being unloaded.
 * @return 0: Success; Anything else: failure.
 */
int scene_free(struct scene *scene);


/**
 * scene_flipCollisionVisibility flips collision from invisible to visible,
 * and vice-versa.
 *
 * @param [in] scene: The scene.
 * @return 0: Success; Anything else: failure.
 */
void scene_flipCollisionVisibility(struct scene *scene);


/**
 * scene_hasLoader checks if the scene has a triggered loader.
 *
 * @param [in] scene: The scene.
 * @return 1: If the scene has a triggered loader; 0: Otherwise.
 */
int scene_hasLoader(struct scene *scene);


/**
 * scene_setLoader assigns a loader to be used in the screen transition.
 *
 * @param [in] scene: The scene.
 * @param [in] entity: The loader for the screen transition.
 */
void scene_setLoader(struct scene *scene, struct entity *entity);


#endif /* !defined(SCENE_H) && !defined(FORWARD_ONLY) */
