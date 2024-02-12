#include <core/core.h>
#include <entity.h>
#include <error.h>
#include <scene.h>

#include <GFraMe/gfmSprite.h>

#include <string.h>


/**
 * The default pre-update function (that does nothing).
 *
 * @param [in] self: The entity itself.
 * @param [in] scene: The scene that called this custom function.
 */
static int _entity_preUpdate(struct entity *self, struct scene *scene) {
	return 0;
}


/**
 * The default post-update function (that does nothing).
 *
 * @param [in] self: The entity itself.
 * @param [in] scene: The scene that called this custom function.
 */
static int _entity_postUpdate(struct entity *self, struct scene *scene) {
	return 0;
}


/**
 * _entity_draw renders the underlying gfmSprite.
 *
 * @param [in] self: The entity itself.
 * @param [in] scene: The scene that called this custom function.
 */
static int _entity_draw(struct entity *self, struct scene *scene) {
	if (self->sprite) {
		return gfmSprite_draw(self->sprite, gameCtx);
	}
	return 0;
}


/**
 * _entity_free releases every resource in the underlying entity.
 *
 * @param [in] self: The entity itself.
 * @param [in] scene: The scene that called this custom function.
 */
static int _entity_free(struct entity *self, struct scene *scene) {
	entity_free(self);
	return 0;
}


/**
 * Initializes an entity,
 * configuring the custom functions as the default ones.
 *
 * The sprite's offset is calculate as to center the hitbox horizontally,
 * but keeping the hitbox at the bottom of the image.
 *
 * @param [in] self: The entity itself.
 * @return 0: Success; Anything else: failure.
 */
int entity_init(
	struct entity *entity
	, int x
	, int y
	, int width
	, int height
	, gfmSpriteset *sset
	, enum type type
) {
	int tileWidth, tileHeight, offX, offY;
	int rv = 1;

	memset(entity, 0x0, sizeof(struct entity));

	ASSERT_OK(gfmSprite_getNew(&entity->sprite), __ret);

	ASSERT_OK(
		gfmSpriteset_getDimension(
			&tileWidth
			, &tileHeight
			, sset
		)
		, __ret
	);

	offX = (width - tileWidth) / 2;
	offY = height - tileHeight;

	ASSERT_OK(
		gfmSprite_init(
			entity->sprite
			, x
			, y
			, width
			, height
			, sset
			, offX
			, offY
			, entity
			, type
		)
		, __ret
	);

	entity->fn.preUpdate = &_entity_preUpdate;
	entity->fn.postUpdate = &_entity_postUpdate;
	entity->fn.draw = &_entity_draw;
	entity->fn.free = &_entity_free;

	entity->type = type;

	rv = 0;
__ret:
	if (rv != 0 && entity->sprite) {
		gfmSprite_free(&entity->sprite);
	}

	return rv;
}


/**
 * entity_free releases every resource associated with an entity.
 *
 * NOTE: This doesn't call entity->fn.free(),
 * so specialized entities may call this function.
 */
void entity_free(struct entity *entity) {
	if (entity->sprite) {
		gfmSprite_free(&entity->sprite);
	}
}
