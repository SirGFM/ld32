#include <core/core.h>
#include <core/types.h>
#include <collision.h>
#include <entity.h>
#include <entities/loader.h>
#include <entities/new_by_type.h>
#include <error.h>
#include <scene.h>
#include <util.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <GFraMe/gfmTilemap.h>

#include <stdlib.h>
#include <string.h>


/**
 * How bigger (in pixel) the quadtree is on each direction (up, down, left, right).
 */
#define QUADTREE_PADDING 16


/**
 * scene_loadScene loads a scene from a pre-loaded map,
 * instantiating objects as needed.
 *
 * @param [in] scene: The struct where the scene is loaded.
 * @param [in] map: The pre-loaded used to load the scene.
 * @return 0: Success; Anything else: failure.
 */
int scene_loadScene(struct scene *scene, struct map *map) {
	struct scene tmp = {0};
	int count, i;
	int rv = 1;
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
			ASSERT_OK(grv = gfmTilemap_getDimension(&tmp.width, &tmp.height, cur), __ret);
		}
		ASSERT_OK(grv = gfmTilemap_setPosition(cur, x, y), __ret);
	}

	/* Initialize the quadtrees and load the static one. */
	ASSERT_OK(grv = gfmQuadtree_getNew(&tmp.staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_getNew(&tmp.dynamicCollider), __ret);

	ASSERT_OK(
		grv = gfmQuadtree_initRoot(
			tmp.staticCollider
			, -QUADTREE_PADDING/*x*/
			, -QUADTREE_PADDING/*y*/
			, tmp.width + (QUADTREE_PADDING * 2)
			, tmp.height + (QUADTREE_PADDING * 2)
			, 8/*depth*/
			, 16/*nodes*/
		)
		, __ret
	);

	ASSERT_OK(grv = gfmQuadtree_setStatic(tmp.staticCollider), __ret);
	ASSERT_OK(grv = gfmQuadtree_enableContinuosCollision(tmp.staticCollider), __ret);

	for (i = 0; i < map->numTilemaps; i++) {
		gfmTilemap *cur = map->tilemaps[i];
		int len;

		ASSERT_OK(grv = gfmTilemap_getAreasLength(&len, cur), __ret);
		if (len > 0) {
			ASSERT_OK(grv = gfmQuadtree_populateTilemap(tmp.staticCollider, cur), __ret);
		}
	}

	/* Load the entities. */
	count = 0;
	for (i = 0; i < map->numObjectLists; i++) {
		count += map->objects[i].count;
	}
	ASSERT((tmp.entities = malloc(sizeof(struct entity*) * count)) != 0, __ret);

	for (i = 0; i < map->numObjectLists; i++) {
		int j;

		for (j = 0; j < map->objects[i].count; j++) {
			struct mapObject *data = map->objects[i].data + j;

			ASSERT_OK(
				rv = entity_newByType(
					tmp.entities + tmp.numEntities
					, data
				)
				, __ret
			);

			tmp.numEntities++;
		}
	}

	for (i = 0; i < tmp.numEntities; i++) {
		struct entity *cur = tmp.entities[i];

		ASSERT_OK(rv = entity_fixSpriteReference(cur), __ret);
		ASSERT_OK(rv = cur->fn.onLoad(cur, &tmp), __ret);
	}

	/* Set the BG color. */
	if (map->bgColor != 0) {
		ASSERT_OK(grv = gfm_setBackground(gameCtx, map->bgColor), __ret);
	}

	tmp.map = map;
	memcpy(scene, &tmp, sizeof(struct scene));
	memset(&tmp, 0, sizeof(struct scene));

__ret:
	scene_free(&tmp);

	return rv | grv;
}


int scene_update(struct scene *scene) {
	int i;
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(grv = gfm_getElapsedTime(&scene->elapsedMs, gameCtx), __ret);

	/* Reset the dynamic quadtree. */
	ASSERT_OK(
		grv = gfmQuadtree_initRoot(
			scene->dynamicCollider
			, -QUADTREE_PADDING/*x*/
			, -QUADTREE_PADDING/*y*/
			, scene->width + (QUADTREE_PADDING * 2)
			, scene->height + (QUADTREE_PADDING * 2)
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
		ASSERT_OK(
			rv = collision_collideSprite(
				scene->staticCollider
				, cur->sprite
			)
			, __ret
		);
		ASSERT_OK(
			rv = collision_collideSprite(
				scene->dynamicCollider
				, cur->sprite
			)
			, __ret
		);
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

#if defined(DEBUG)
	if (scene->drawCollisions) {
		grv = gfmQuadtree_drawBounds(scene->staticCollider, gameCtx, 0);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_QUADTREE_EMPTY, __ret);

		grv = gfmQuadtree_drawBounds(scene->dynamicCollider, gameCtx, 0);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_QUADTREE_EMPTY, __ret);
	}
#endif /* defined(DEBUG) */

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


int scene_getCameraTransitionPosition(
	int *x
	, int *y
	, struct scene *self
	, struct scene *other
	, int doorID
) {
	gfmCamera *camera;
	int i;
	int cameraW, cameraH;
	int doorCenterX, doorCenterY;
	int offsetX, offsetY;
	int otherW, otherH;
	int selfW, selfH;
	int rv = 1;
	gfmRV grv = GFMRV_OK;

	/* Get the camera dimensions. */
	ASSERT_OK(grv = gfm_getCamera(&camera, gameCtx), __ret);
	ASSERT_OK(grv = gfmCamera_getDimensions(&cameraW, &cameraH, camera), __ret);

	/* Look for the target door in the new scene. */
	for (i = 0; i < self->numEntities; i++) {
		struct entity *entity = self->entities[i];

		if (entity->type != TYP_LOADER || !loader_isID(entity, doorID)) {
			continue;
		}

		ASSERT_OK(
			grv = gfmSprite_getCenter(
				&doorCenterX
				, &doorCenterY
				, entity->sprite
			)
			, __ret
		);
		break;
	}
	ASSERT(i < self->numEntities, __ret);

	/* Get the map dimensions from any tilemap. */
	ASSERT_OK(
		grv = gfmTilemap_getDimension(
			&selfW
			, &selfH
			, self->map->tilemaps[0]
		)
		, __ret
	);
	ASSERT_OK(
		grv = gfmTilemap_getDimension(
			&otherW
			, &otherH
			, self->map->tilemaps[0]
		)
		, __ret
	);

	/* Get the map position in world space. */
	offsetX = self->map->offsetX - other->map->offsetX;
	offsetY = self->map->offsetY - other->map->offsetY;

	/* Calculate the target camera position. */
	if (doorCenterX == 0 || doorCenterX == selfW) {
		if (doorCenterX == 0) {
			/* Loader is on the left wall of the new scene. */
			*x = otherW;
		}
		else if (doorCenterX == selfW) {
			/* Loader is on the right wall of the new scene. */
			*x = -cameraW;
		}

		*y = offsetY + doorCenterY - cameraH / 2;
	}
	else if (doorCenterY == 0 || doorCenterY == selfH) {
		if (doorCenterY == 0) {
			/* Loader is on the top wall of the new scene. */
			*y = otherH;
		}
		else if (doorCenterY == selfH) {
			/* Loader is on the bottom wall of the new scene. */
			*y = -cameraH;
		}

		*x = offsetX + doorCenterX - cameraW / 2;
	}
	else {
		int invalid_loader_position = 0;
		ASSERT(invalid_loader_position, __ret);
	}

	/* Adjust the camera position so it's within/bound to the new scene. */
	*x = max(min(*x, offsetX), offsetX + selfW - cameraW);
	*y = max(min(*y, offsetY), offsetY + selfH - cameraH);

	rv = 0;
__ret:
	return rv | grv;
}


int scene_loadSceneFromFile(struct scene *scene, char *dir, int len) {
	struct map *map;
	struct scene tmp = {0};
	int rv = 1;

	ASSERT(map = malloc(sizeof(struct map)), __ret);
	memset(map, 0x0, sizeof(struct map));

	ASSERT_OK(rv = map_loadMap(map, dir, len), __ret);
	ASSERT_OK(rv = scene_loadScene(&tmp, map), __ret);

	map = 0;
	tmp.fromFile = 1;
	memcpy(scene, &tmp, sizeof(tmp));
	memset(&tmp, 0x0, sizeof(tmp));

__ret:
	scene_free(&tmp);
	if (map != 0) {
		map_free(map);
		free(map);
	}

	return rv;
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

	if (scene->staticCollider) {
		ASSERT_OK(grv = gfmQuadtree_free(&scene->staticCollider), __ret);
	}
	if (scene->dynamicCollider) {
		ASSERT_OK(grv = gfmQuadtree_free(&scene->dynamicCollider), __ret);
	}

	if (scene->fromFile) {
		map_free(scene->map);
		free(scene->map);
		scene->map = 0;
	}

__ret:
	return rv | grv;
}


void scene_flipCollisionVisibility(struct scene *scene) {
	scene->drawCollisions = !scene->drawCollisions;
}


int scene_hasLoader(struct scene *scene) {
	return scene->curLoader != 0;
}


void scene_setLoader(struct scene *scene, struct entity *entity) {
	scene->curLoader = entity;
}
