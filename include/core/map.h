#ifndef MAP_H
#define MAP_H


#include <core/attributes.h>
#include <core/types.h>

#include <GFraMe/gfmTilemap.h>


struct mapAttribute {
	/** The attribute's type/identifier. */
	enum attribute type;
	/**
	 * The attribute's value.
	 *
	 * For string attributes,
	 * this is the index which should be passed to store_getStr.
	 */
	int value;
};


struct mapObject {
	/** The object's type. */
	enum type type;
	/** The object's horizontal position. */
	int x;
	/** The object's vertical position. */
	int y;
	/** The object's width. */
	int width;
	/** The object's height. */
	int height;
	/** The list of attributes in this object. */
	struct mapAttribute *attributes;
	/** How many attributes there are in this object. */
	int numAttribute;
};


struct objectList {
	/** How many objects there are in this list. */
	int count;
	/** The objects. */
	struct mapObject *data;
};


struct map {
	/** List of objects in this map. */
	struct objectList *objects;
	/** List of tilemaps, in the order they should be rendered. */
	gfmTilemap **tilemaps;
	/** How many tilemaps there are in this map. */
	int numTilemaps;
	/** How many object lists there are in this map. */
	int numObjectLists;
	/** The background color for the current map. */
	int bgColor;
};


/**
 * map_loadMap loads a map structured into multiple files within a directory.
 *
 * The map is loaded based on the directory's name,
 * which is assumed to be within an 'assets/' directory.
 * E.g., to load the map 'assets/maps/cave/',
 * 'maps/cave/' must be passed as the map directory!
 *
 * This map must have a 'meta.txt' metadata file listing the map's components.
 *
 * @param [out] map: The loaded map.
 * @param [in] mapDir: The map directory.
 * @param [in] len: mapDir's length.
 * @return 0: Success; Anything else: failure.
 */
int map_loadMap(struct map *map, char *mapDir, int len);


/**
 * map_free releases the memory associated with the loaded map.
 *
 * NOTE: Only the map's contents are released!
 * The region used by the map structure itself is cleared/set to 0,
 * but it isn't freed!
 *
 * @param [in] map: The map to be freed.
 */
void map_free(struct map *map);


#endif /* MAP_H */
