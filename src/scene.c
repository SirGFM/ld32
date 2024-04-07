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

	/* Position every map and retrieve the map's dimensions,
	 * assuming that every maps has the same dimension. */
	for (i = 0; i < map->numTilemaps; i++) {
		gfmTilemap *cur;
		int x, y;

		cur = map->tilemaps[i];
		x = map->offsets[i].x;
		y = map->offsets[i].y;

		if (i == 0) {
			ASSERT_OK(grv = gfmTilemap_getDimension(&width, &height, cur), __ret);
		}
		ASSERT_OK(grv = gfmTilemap_setPosition(cur, x, y), __ret);
	}

	/* Initialize the quadtrees and load the static one. */
	ASSERT_OK(grv = gfmQuadtree_getNew(&staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_getNew(&dynamicCollider), __ret);

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


int scene_draw(struct scene *scene) {
	int i;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

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


int scene_setRelativePosition(
	struct scene *self
	, struct scene *other
	, enum scene_relativePosition pos
	, int doorOffsetX
	, int doorOffsetY
) {
	int i, offsetX, offsetY, otherW, otherH;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* Get self's position relative to other. */
	if (pos != SCENE_RESET) {
		ASSERT_OK(
			grv = gfmTilemap_getDimension(
				&otherW
				, &otherH
				, other->map->tilemaps[0]
			)
			, __ret
		);
	}

	switch (pos) {
	case SCENE_RESET:
		offsetX = 0;
		offsetY = 0;
		break;
	case SCENE_LEFT_OF:
		offsetX = -otherW;
		offsetY = 0;
		break;
	case SCENE_RIGHT_OF:
		offsetX = otherW;
		offsetY = 0;
		break;
	case SCENE_ABOVE:
		offsetX = 0;
		offsetY = -otherH;
		break;
	case SCENE_BELLOW:
		offsetX = 0;
		offsetY = otherH;
		break;
	}

	/* Move self to its new position. */
	for (i = 0; i < self->map->numTilemaps; i++) {
		gfmTilemap *cur;
		int x, y;

		cur = self->map->tilemaps[i];
		x = (self->map->offsets[i].x + doorOffsetX) * TILE_WIDTH + offsetX;
		y = (self->map->offsets[i].y + doorOffsetY) * TILE_HEIGHT + offsetY;

		ASSERT_OK(grv = gfmTilemap_setPosition(cur, x, y), __ret);
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
