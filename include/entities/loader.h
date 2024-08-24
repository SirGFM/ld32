#ifndef LOADER_H
#define LOADER_H


#include <core/map.h>
#include <entity.h>


enum loaderState {
	LOADER_IDLE = 0
	, LOADER_TRIGGERED
	, LOADER_LOADING
};


struct loader {
	/** The base entity. */
	struct entity base;
	/** Name of the target map. */
	char *name;
	/** Name's length. */
	int nameLen;
	/** Handle for the target map's name in the store. */
	int storeId;
	/**
	 * The loader's identifier,
	 * so an entrance in one scene may match the exit in another.
	 */
	int id;
	/**
	 * Track whether the loader has been touched/
	 * its stage is being loaded.
	 */
	enum loaderState state;
};


/**
 * loader_new allocates and initializes a new loader.
 *
 * @param [out] entity: The new loader.
 * @param [in] data: The option's configuration.
 * @return 0: Success; Anything else: failure.
 */
int loader_new(struct entity **entity, struct mapObject *data);


/**
 * loader_onTouch prepares a loader to starting loading,
 * after it's touched by a valid entity.
 *
 * @param [in] entity: The touched loader.
 */
void loader_onTouch(struct entity *entity);


#endif /* LOADER_H */
