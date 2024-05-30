#ifndef PRELOAD_MAP_H
#define PRELOAD_MAP_H

#include <core/map.h>


/**
 * preloadedMap_getName retrieves the name of the preloaded map.
 *
 * @param [out] name: The name of the preloaded map.
 * @param [out] len: name's length.
 * @param [in] idx: The preloaded map's index.
 * @return 0: Success; Anything else: failure.
 */
int preloadedMap_getName(char **name, int *len, int idx);


/**
 * preloadedMap_getCount retrieves the number of preloaded maps.
 *
 * @param [out] count: The number of preloaded maps.
 * @return 0: Success; Anything else: failure.
 */
int preloadedMap_getCount(int *count);


/**
 * preloadMap_loadAll loads the predefined maps.
 *
 * @return 0: Success; Anything else: failure.
 */
int preloadMap_loadAll();


/**
 * preloadMap_free releases every preloaded map.
 *
 * @return 0: Success; Anything else: failure.
 */
int preloadMap_free();


/**
 * preloadMap_getMapByName retrieves the preloaded map identified by the name.
 *
 * @param [out] map: The preloaded map.
 * @param [in] name: The name of the preloaded map.
 * @param [in] len: name's length.
 * @return 0: Success; Anything else: failure.
 */
int preloadMap_getMapByName(struct map **map, char *name, int len);


/**
 * preloadMap_getMapByHandle retrieves the preloaded map identified by the stored handle.
 *
 * @param [out] map: The preloaded map.
 * @param [in] idx: The identifier of the map's name in the store.
 * @return 0: Success; Anything else: failure.
 */
int preloadMap_getMapByHandle(struct map **map, int id);


#endif /* PRELOAD_MAP_H */
