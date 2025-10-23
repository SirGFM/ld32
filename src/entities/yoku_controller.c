#include <error.h>
#include <core/assets.h>
#include <core/attributes.h>
#include <core/map.h>
#include <core/types.h>
#include <scene.h>
#include <entities/yoku_block.h>
#include <entities/yoku_controller.h>
#include <util.h>

#include <stdlib.h>
#include <string.h>


/**
 * yokuController_noop implements a no-op handler.
 *
 * @param [in] entity: The yoku controller's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuController_noop(struct entity *entity, struct scene *scene) {
	return 0;
}


/**
 * yokuController_onLoad configures every associated yoku block in the scene.
 *
 * @param [in] entity: The yoku controller's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuController_onLoad(struct entity *entity, struct scene *scene) {
	struct yokuController *controller = (struct yokuController*)entity;
	int rv = 1;
	int numSteps = 0;
	int i;

	for (i = 0; i < scene->numEntities; i++) {
		struct yokuBlock *block;
		struct entity *cur = scene->entities[i];

		if (cur->type != TYP_YOKU_BLOCK) {
			continue;
		}

		block = (struct yokuBlock*)cur;
		if (block->parentID != controller->id) {
			continue;
		}

		numSteps = max(numSteps, block->id + block->durationSteps + 1);
	}

	for (i = 0; i < scene->numEntities; i++) {
		struct yokuBlock *block;
		struct entity *cur = scene->entities[i];

		if (cur->type != TYP_YOKU_BLOCK) {
			continue;
		}

		block = (struct yokuBlock*)cur;
		if (block->parentID != controller->id) {
			continue;
		}

		yokuBlock_setTiming(cur, numSteps, controller->cycleMs);
	}

	rv = 0;
__ret:
	return rv;
}


/**
 * yokuController_staticFree releases every resource allocated into a yoku controller,
 * except by the yoku controller's memory itself.
 *
 * Therefore, this is supposed to be called on a stack-allocated yoku controller.
 *
 * @param [in] yokuController: The yoku controller's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int yokuController_staticFree(struct yokuController *controller) {
	entity_free(&controller->base);
	return 0;
}


/**
 * yokuController_free releases every resource allocated by yokuController_new.
 *
 * @param [in] entity: The yoku controller's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int yokuController_free(struct entity *entity, struct scene *scene) {
	struct yokuController *controller = (struct yokuController*)entity;

	yokuController_staticFree(controller);

	free(controller);
	return 0;
}


int yokuController_new(struct entity **entity, struct mapObject *data) {
	struct entity *ret = 0;
	struct yokuController tmp = {0};
	int rv = 1;
	int i;

	/* Load the loader's type and index. */
	tmp.id = -1;
	tmp.cycleMs = -1;
	ASSERT(data->numAttribute == 2, __ret);
	for (i = 0; i < data->numAttribute; i++) {
		if (data->attributes[i].type == ATTR_ID) {
			tmp.id = data->attributes[i].value;
		}
		else if (data->attributes[i].type == ATTR_DURATION) {
			tmp.cycleMs = data->attributes[i].value;
		}
	}
	ASSERT(tmp.id >= 0, __ret);
	ASSERT(tmp.cycleMs > 0, __ret);

	/* Load a dummy sprite (required by the entity). */
	ASSERT_OK(
		entity_init(
			&tmp.base
			, data->x
			, data->y
			, data->width
			, data->height
			, gfx8x8
			, TYP_YOKU_CONTROLLER
		)
		, __ret
	);

	/* Configure the vtable. */
	tmp.base.fn.onLoad = yokuController_onLoad;
	tmp.base.fn.preUpdate = yokuController_noop;
	tmp.base.fn.postUpdate = yokuController_noop;
	tmp.base.fn.draw = yokuController_noop;
	tmp.base.fn.free = yokuController_free;

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

	yokuController_staticFree(&tmp);
	return rv;
}
