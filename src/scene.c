#include <core/core.h>
#include <entity.h>
#include <error.h>
#include <scene.h>

#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTilemap.h>

#include <stdlib.h>
#include <string.h>


/**
 * scene_loadScene loads a scene from a pre-loaded map,
 * instantiating objects as needed.
 *
 * @param [in] scene: The struct where the scene is loaded.
 * @param [in] map: The pre-loaded used to load the scene.
 * @return 0: Success; Anything else: failure.
 */
int scene_loadScene(struct scene *scene, struct map *map) {
	gfmQuadtreeRoot *staticCollider;
	gfmQuadtreeRoot *dynamicCollider;
	int i;
	int height = 0, width = 0;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* Initialize the quadtrees and calculate the map's dimensions. */
	ASSERT_OK(grv = gfmQuadtree_getNew(&staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_getNew(&dynamicCollider), __ret);

	for (i = 0; i < map->numTilemaps; i++) {
		gfmTilemap *cur = map->tilemaps[i];
		int tmpW, tmpH;

		ASSERT_OK(grv = gfmTilemap_getDimension(&tmpW, &tmpH, cur), __ret);
		if (tmpW > width) {
			width = tmpW;
		}
		if (tmpH > height) {
			height = tmpH;
		}
	}

	/* Load the static quadtree. */
	ASSERT_OK(
		grv = gfmQuadtree_initRoot(
			staticCollider
			, -16/*x*/
			, -16/*y*/
			, width
			, height
			, 8/*depth*/
			, 16/*nodes*/
		)
		, __ret
	);

	ASSERT_OK(grv = gfmQuadtree_setStatic(staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_enableContinuosCollision(staticCollider), __ret);

	for (i = 0; i < map->numTilemaps; i++) {
		gfmTilemap *cur = map->tilemaps[i];

		ASSERT_OK(grv = gfmQuadtree_populateTilemap(staticCollider, cur), __ret);
	}

	/* TODO: Load the entities. */

	return -1;

	scene->map = map;
	scene->staticCollider = staticCollider;
	scene->dynamicCollider = dynamicCollider;
	scene->width = width;
	scene->height = height;

	staticCollider = 0;
	dynamicCollider = 0;
__ret:
	if (staticCollider) {
		gfmQuadtree_free(&staticCollider);
	}
	if (dynamicCollider) {
		gfmQuadtree_free(&dynamicCollider);
	}

	return rv | grv;
}


int scene_update(struct scene *scene) {
	int i;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* Reset the dynamic quadtree. */
	ASSERT_OK(
		grv = gfmQuadtree_initRoot(
			scene->dynamicCollider
			, -16/*x*/
			, -16/*y*/
			, scene->width
			, scene->height
			, 8/*depth*/
			, 16/*nodes*/
		)
		, __ret
	);

	/* Update any tilemap animation. */
	for (i = 0; i < scene->map->numTilemaps; i++) {
		gfmTilemap *cur = scene->map->tilemaps[i];

		ASSERT_OK(grv = gfmTilemap_update(cur, gameCtx), __ret);
	}

	/* Update and collide everything. */
	for (i = 0; i < scene->numEntities; i++) {
		struct entity *cur = scene->entities[i];

		ASSERT_OK(rv = cur->fn.preUpdate(cur, scene), __ret);
		ASSERT_OK(grv = gfmSprite_update(cur->sprite, gameCtx), __ret);
		/* TODO: Collide. */
	}

	/* Execute any custom post-update. */
	for (i = 0; i < scene->numEntities; i++) {
		struct entity *cur = scene->entities[i];

		ASSERT_OK(rv = cur->fn.postUpdate(cur, scene), __ret);
	}

__ret:
	return rv | grv;
}


int scene_draw(struct scene *scene, int x, int y) {
	int i;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* TODO: Adjust the camera. */

	for (i = 0; i < scene->map->numTilemaps; i++) {
		gfmTilemap *cur = scene->map->tilemaps[i];

		ASSERT_OK(grv = gfmTilemap_draw(cur, gameCtx), __ret);
	}

	for (i = 0; i < scene->numEntities; i++) {
		struct entity *cur = scene->entities[i];

		ASSERT_OK(rv = cur->fn.draw(cur, scene), __ret);
	}

__ret:
	return rv | grv;
}


int scene_free(struct scene *scene) {
	int i;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	for (i = 0; i < scene->numEntities; i++) {
		struct entity *cur = scene->entities[i];

		ASSERT_OK(rv = cur->fn.free(cur, scene), __ret);
	}
	free(scene->entities);
	scene->entities = 0;

	ASSERT_OK(grv = gfmQuadtree_free(&scene->staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_free(&scene->dynamicCollider), __ret);

__ret:
	return rv | grv;
}
