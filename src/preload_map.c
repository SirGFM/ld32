#include <core/map.h>
#include <core/store.h>
#include <error.h>
#include <preload_map.h>

#include <stdlib.h>
#include <string.h>


struct mapEntry {
	/** The preloaded map. */
	struct map map;
	/** The map name identifier in the store. */
	int storeId;
};


struct mapStore {
	/** The list of maps. */
	struct mapEntry *list;
	/** The number of entries stored in the list. */
	int len; \
	/** The number of slots in the list. */\
	int cap;
};

struct mapStore maps = {0};


int _preloadMap_free(struct mapStore *maps) {
	int i;

	for (i = 0; i < maps->len; i++) {
		map_free(&maps->list[i].map);
	}

	if (maps->list) {
		free(maps->list);
		maps->list = 0;
	}

	return 0;
}


int preloadMap_loadAll() {
	struct mapStore tmp = {0};
	int size, i;
	int rv = 1;

	ASSERT_OK(preloadedMap_getCount(&tmp.cap), __ret);

	size = sizeof(struct mapEntry) * tmp.cap;
	ASSERT(tmp.list = malloc(size), __ret);
	memset(tmp.list, 0, size);

	for (i = 0; i < tmp.cap; i++) {
		struct mapEntry *entry = tmp.list + i;
		char *name;
		int len;

		ASSERT_OK(preloadedMap_getName(&name, &len, i), __ret);
		ASSERT_OK(store_addStr(&entry->storeId, name, len), __ret);
		ASSERT_OK(map_loadMap(&entry->map, name, len), __ret);

		tmp.len++;
	}

	memcpy(&maps, &tmp, sizeof(tmp));
	memset(&tmp, 0, sizeof(tmp));

	rv = 0;
__ret:
	_preloadMap_free(&tmp);

	return 0;
}


int preloadMap_free() {
	return _preloadMap_free(&maps);
}


int preloadMap_getMapByName(struct map **map, char *name, int len) {
	int rv = 1;
	int i;

	for (i = 0; i < maps.len; i++) {
		char *wantName;
		int wantLen;

		ASSERT_OK(preloadedMap_getName(&wantName, &wantLen, i), __ret);
		if (wantLen == len && 0 == memcmp(wantName, name, len)) {
			*map = &maps.list[i].map;
			break;
		}
	}
	ASSERT(i < maps.len, __ret);

	rv = 0;
__ret:
	return rv;
}


int preloadMap_getMapByHandle(struct map **map, int id) {
	int rv = 1;
	int i;

	for (i = 0; i < maps.len; i++) {
		if (maps.list[i].storeId == id) {
			*map = &maps.list[i].map;
			break;
		}
	}
	ASSERT(i < maps.len, __ret);

	rv = 0;
__ret:
	return rv;
}
