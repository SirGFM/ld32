#include <error.h>
#include <core/assets.h>
#include <core/attributes.h>
#include <core/core.h>
#include <core/map.h>
#include <core/types.h>
#include <scene.h>
#include <entities/yoku_block.h>

#include <stdlib.h>
#include <string.h>


/**
 * yokuBlock_noop implements a no-op handler.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuBlock_noop(struct entity *entity, struct scene *scene) {
	return 0;
}


/**
 * yokuBlock_staticFree releases every resource allocated into a yoku block,
 * except by the yoku block's memory itself.
 *
 * Therefore, this is supposed to be called on a stack-allocated yoku block.
 *
 * @param [in] yokuBlock: The yoku block's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int yokuBlock_staticFree(struct yokuBlock *block) {
	entity_free(&block->base);
	return 0;
}


/**
 * yokuBlock_free releases every resource allocated by yokuBlock_new.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuBlock_free(struct entity *entity, struct scene *scene) {
	struct yokuBlock *block = (struct yokuBlock*)entity;

	yokuBlock_staticFree(block);

	free(block);
	return 0;
}


/**
 * yokuBlock_preUpdate updates the yoku block's timer,
 * making it phase in and out.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuBlock_preUpdate(struct entity *entity, struct scene *scene) {
	struct yokuBlock *block = (struct yokuBlock*)entity;
	int rv = 1;

	if (block->preSleepMs > 0) {
		block->preSleepMs -= scene->elapsedMs;
		if (block->preSleepMs < 0) {
			block->timer -= block->preSleepMs;
		}
	} else {
		block->timer += scene->elapsedMs;
		if (block->timer >= block->activeMs + block->sleepMs) {
			block->timer -= block->activeMs + block->sleepMs;
		}
	}

	rv = 0;
__ret:
	return rv;
}


/**
 * yokuBlock_draw draws the current yoku block.
 *
 * @param [in] entity: The yoku block's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuBlock_draw(struct entity *entity, struct scene *scene) {
	int rv = 1;

	if (yokuBlock_isActive(entity)) {
		ASSERT(GFMRV_OK == gfmSprite_draw(entity->sprite, gameCtx), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


int yokuBlock_new(struct entity **entity, struct mapObject *data) {
	struct entity *ret = 0;
	struct yokuBlock tmp = {0};
	int rv = 1;
	int i;

	/* Load the loader's type and index. */
	tmp.id = -1;
	tmp.parentID = -1;
	tmp.durationSteps = -1;
	ASSERT(data->numAttribute == 3, __ret);
	for (i = 0; i < data->numAttribute; i++) {
		if (data->attributes[i].type == ATTR_ID) {
			tmp.id = data->attributes[i].value;
		}
		else if (data->attributes[i].type == ATTR_PARENT_ID) {
			tmp.parentID = data->attributes[i].value;
		}
		else if (data->attributes[i].type == ATTR_DURATION) {
			tmp.durationSteps = data->attributes[i].value;
		}
	}
	ASSERT(tmp.id >= 0, __ret);
	ASSERT(tmp.parentID >= 0, __ret);
	ASSERT(tmp.durationSteps > 0, __ret);

	ASSERT_OK(
		entity_init(
			&tmp.base
			, data->x
			, data->y
			, data->width
			, data->height
			, gfx16x16
			, TYP_YOKU_BLOCK
		)
		, __ret
	);

	ASSERT(GFMRV_OK == gfmSprite_setFrame(tmp.base.sprite, 107), __ret);
	ASSERT(GFMRV_OK == gfmSprite_setFixed(tmp.base.sprite), __ret);

	/* Configure the vtable. */
	tmp.base.fn.preUpdate = yokuBlock_preUpdate;
	tmp.base.fn.postUpdate = yokuBlock_noop;
	tmp.base.fn.draw = yokuBlock_draw;
	tmp.base.fn.free = yokuBlock_free;

	/* Return the initialized object. */
	ASSERT((ret = malloc(sizeof(tmp))) != 0, __ret);
	memcpy(ret, &tmp, sizeof(tmp));
	memset(&tmp, 0x0, sizeof(tmp));

	*entity = ret;
	ret = 0;

	rv = 0;
__ret:
	if (ret) {
		free(ret);
	}

	yokuBlock_staticFree(&tmp);
	return rv;
}


void yokuBlock_setTiming(struct entity *entity, int numSteps, int cycleMs) {
	struct yokuBlock *block = (struct yokuBlock*)entity;
	int stepMs;

	stepMs = cycleMs / numSteps;

	block->preSleepMs = block->id * stepMs;
	block->activeMs = stepMs * block->durationSteps;
	block->sleepMs = (numSteps - block->durationSteps) * stepMs;
}


int yokuBlock_isActive(struct entity *entity) {
	struct yokuBlock *block = (struct yokuBlock*)entity;

	return block->preSleepMs <= 0 && block->timer < block->activeMs;
}
