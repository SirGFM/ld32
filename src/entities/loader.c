#include <error.h>
#include <core/assets.h>
#include <core/attributes.h>
#include <core/map.h>
#include <core/store.h>
#include <core/types.h>
#include <scene.h>
#include <entities/loader.h>
#include <mainloop.h>
#include <preload_map.h>

#include <stdlib.h>
#include <string.h>


/**
 * loader_noop implements a no-op handler.
 *
 * @param [in] entity: The loader's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int loader_noop(struct entity *entity, struct scene *scene) {
	return 0;
}


/**
 * loader_static_free releases every resource allocated into a loader,
 * except by the loader's memory itself.
 *
 * Therefore, this is supposed to be called on a stack-allocated loader.
 *
 * @param [in] loader: The loader's embedded entity.
 * @return 0: Success; Anything else: failure.
 */
static int loader_static_free(struct loader *loader) {
	entity_free(&loader->base);
	return 0;
}


/**
 * loader_free releases every resource allocated by loader_new.
 *
 * @param [in] entity: The loader's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int loader_free(struct entity *entity, struct scene *scene) {
	struct loader *loader = (struct loader*)entity;

	loader_static_free(loader);

	free(loader);
	return 0;
}


/**
 * loader_postUpdate starts a scene transition if the loader was touched this frame.
 *
 * @param [in] entity: The loader's embedded entity.
 * @param [in] scene: The scene that called this function.
 * @return 0: Success; Anything else: failure.
 */
static int loader_postUpdate(struct entity *entity, struct scene *scene) {
	struct loader *loader = (struct loader*)entity;
	int rv = 1;

	if (loader->state == LOADER_TRIGGERED && !scene_hasLoader(scene)) {
		struct map *map;

		scene_setLoader(scene, entity);
		/* TODO: Use preloadMap_getMapByHandle */
		ASSERT_OK(preloadMap_getMapByName(&map, loader->name, loader->nameLen), __ret);
		ASSERT_OK(mainloop_transitionSwapScene(map, loader->id), __ret);

		loader->state = LOADER_LOADING;
	}

	rv = 0;
__ret:
	return rv;
}


int loader_new(struct entity **entity, struct mapObject *data) {
	char *tag;
	struct entity *ret = 0;
	struct loader tmp = {0};
	int rv = 1;
	int i;

	/* Load the loader's type and index. */
	tmp.id = -1;
	ASSERT(data->numAttribute == 2, __ret);
	for (i = 0; i < data->numAttribute; i++) {
		if (data->attributes[i].type == ATTR_ID) {
			tmp.id = data->attributes[i].value;
		}
		else if (data->attributes[i].type == ATTR_MAP) {
			tmp.storeId = data->attributes[i].value;

			ASSERT_OK(store_getStr(&tmp.name, &tmp.nameLen, tmp.storeId), __ret);
		}
	}
	ASSERT(tmp.id >= 0, __ret);
	ASSERT(tmp.nameLen > 0, __ret);

	/* Load a dummy sprite (required by the entity). */
	ASSERT_OK(
		entity_init(
			&tmp.base
			, data->x
			, data->y
			, data->width
			, data->height
			, gfx8x8
			, TYP_LOADER
		)
		, __ret
	);

	/* Configure the vtable. */
	tmp.base.fn.preUpdate = loader_noop;
	tmp.base.fn.postUpdate = loader_postUpdate;
	tmp.base.fn.draw = loader_noop;
	tmp.base.fn.free = loader_free;

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

	loader_static_free(&tmp);
	return rv;
}


void loader_onTouch(struct entity *entity) {
	struct loader *loader = (struct loader*)entity;

	if (loader->state == LOADER_IDLE) {
		loader->state = LOADER_TRIGGERED;
	}
}


int loader_isID(struct entity *entity, int id) {
	struct loader *loader = (struct loader*)entity;

	return loader->id == id;
}
