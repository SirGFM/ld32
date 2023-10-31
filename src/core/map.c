#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>

#include <core/assets.h>
#include <core/attributes.h>
#include <core/core.h>
#include <core/map.h>
#include <core/store.h>
#include <core/types.h>
#include <error.h>
#include <util.h>

#include <stdlib.h>
#include <string.h>


enum metadataFlags {
	META_IS_OBJECT = 0x0001
	, META_IS_COUNT = 0x0002
	, META_IS_TILEMAP = 0x0004
	, META_HAS_COLOR = 0x0008
};


/**
 * mapMetadata represents one line read from the metadata file.
 */
struct mapMetadata {
	/** The null-terminated filename (without its directory). */
	char *file;
	/** The number of tilemaps in this map. */
	int layerCount;
	/** The number of object files for this map. */
	int objCount;
	/** The map's background color (in AARRGGBB). */
	int bgColor;
	/** Which fields were set in this structure. */
	enum metadataFlags flags;
};


/**
 * map_freeObj releases the memory associated with the loaded object.
 *
 * @param [in] obj: The object to be freed.
 */
static void map_freeObj(struct mapObject *obj) {
	if (obj->attributes) {
		free(obj->attributes);
		obj->attributes = 0;
	}
	obj->numAttribute = 0;
}


/**
 * map_freeObjectList releases the list of objects.
 *
 * @param [in] list: The list of objects to be freed.
 */
static void map_freeObjectList(struct objectList *list) {
	int i;

	for (i = 0; i < list->count; i++) {
		map_freeObj(list->data + i);
	}
	if (list->data) {
		free(list->data);
	}
	list->data = 0;
	list->count = 0;
}


/**
 * map_concatPath allocates a new path concatenating the two supplied paths.
 *
 * This function completely ignores the existance null-terminator!
 * Thus, if base is null-terminated,
 * the concatenated path will end up with a '\0' in the middle of the path!
 *
 * @param [out] dst: The allocated memory with the concatenated paths.
 * @param [out] dstLen: The length of the allocated memory.
 * @param [in] base: The initial part of the path.
 * @param [in] baseLen: base's length.
 * @param [in] part: The final part of the path.
 * @param [in] partLen: part's length.
 * @return 0: Success; Anything else: failure.
 */
static int map_concatPath(
	char **dst
	, int *dstLen
	, char *base
	, int baseLen
	, char *part
	, int partLen
) {
	/** The complete path. */
	char *str;
	/** Length of str. */
	int len;
	/**  The return value (initialized to an error). */
	int rv = 1;

	len = baseLen + partLen + 1;
	ASSERT(str = malloc(sizeof(char) * len), __ret);

	memcpy(str, base, sizeof(char) * baseLen);
	str[baseLen] = '/';
	memcpy(str + baseLen + 1, part, sizeof(char) * partLen);

	*dstLen = len;
	*dst = str;
	rv = 0;
__ret:
	return rv;
}


/**
 * map_openParser loads a file from a map into a previously allocated parser.
 *
 * NOTE: mapDir must not be null-terminated!
 *
 * @param [out] parser: The parser in which the file is loaded.
 * @param [in] mapDir: The map's directory.
 * @param [in] dirLen: mapDir's length.
 * @param [in] file: The file to be loaded into the parser.
 * @param [in] fileLen: file's length.
 * @return 0: Success; Anything else: failure.
 */
static int map_openParser(gfmParser *parser, char *mapDir, int dirLen, char *file, int fileLen) {
	/** The complete path to the file within 'assets'. */
	char *filename = 0;
	/** Length of filename. */
	int len;
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(
		rv = map_concatPath(
			&filename
			, &len
			, mapDir
			, dirLen
			, file
			, fileLen
		)
		, __ret
	);

	ASSERT_OK(grv = gfmParser_init(parser, gameCtx, filename, len), __ret);

__ret:
	if (filename) {
		free(filename);
	}

	return rv || grv;
}


/**
 * map_openParserStaticFile loads a file from a map into a previously allocated parser.
 *
 * This function assumes file is a static string
 * to automatically calculate it's length.
 *
 * NOTE: mapDir must not be null-terminated!
 *
 * @param [out] parser: The parser in which the file is loaded.
 * @param [in] mapDir: The map's directory.
 * @param [in] dirLen: mapDir's length.
 * @param [in] file: The file to be loaded into the parser.
 * @return 0: Success; Anything else: failure.
 */
#define map_openParserStaticFile(parser, mapDir, dirLen, file) \
	map_openParser(parser, mapDir, dirLen, file, sizeof(file) - 1)


/**
 * map_parseCount parse's a metadata's count into a map.
 *
 * This function allocates the map's list, but keeping them empty.
 *
 * @param [out] map: The map whose lists will be allocated.
 * @param [in] meta: The metadata with the map's count.
 * @return 0: Success; Anything else: failure.
 */
static int map_parseCount(struct map *map, struct mapMetadata *meta) {
	/**  The return value (initialized to an error). */
	int rv = 1;

	ASSERT(
		map->objects = malloc(sizeof(struct objectList) * meta->objCount)
		, __ret
	);
	memset(map->objects, 0, sizeof(struct objectList) * meta->objCount);
	map->numObjectLists = meta->objCount;

	ASSERT(
		map->tilemaps = malloc(sizeof(gfmTilemap*) * meta->layerCount)
		, __ret
	);
	memset(map->tilemaps, 0, sizeof(gfmTilemap*) * meta->layerCount);
	map->numTilemaps = meta->layerCount;

	rv = 0;
__ret:
	return rv;
}


/**
 * map_parseTilemap parses a tilemap into the map.
 *
 * @param [out] map: The map that will receive the tilemap.
 * @param [in] meta: The metadata with the tilemap's information.
 * @param [in] mapDir: The map's directory.
 * @param [in] dirLen: dirLen's length.
 * @return 0: Success; Anything else: failure.
 */
static int map_parseTilemap(
	struct map *map
	, struct mapMetadata *meta
	, char *mapDir
	, int dirLen
) {
	/** The recently loaded tilemap. */
	gfmTilemap *tilemap = 0;
	/** The tilemap to be loaded. */
	char *filename = 0;
	/** Length of filename. */
	int len;
	/** Index of the available tilemap where the loaded one will be stored. */
	int i;
	/**  The return value (initialized to an error). */
	int rv = 1;
	gfmRV grv = GFMRV_OK;

	/* Find and empty entry for the tilemap. */
	for (i = 0; i < map->numTilemaps && map->tilemaps[i]; i++) {}
	ASSERT(i < map->numTilemaps, __ret);

	ASSERT_OK(
		map_concatPath(
			&filename
			, &len
			, mapDir
			, dirLen
			, meta->file
			, strlen(meta->file)
		)
		, __ret
	);

	ASSERT_OK(grv = gfmTilemap_getNew(&tilemap), __ret);
    ASSERT_OK(
		grv = gfmTilemap_init(
			tilemap
			, gfx8x8
			, 1/*width*/
			, 1/*height*/
			, -1/*defTile*/
		)
		, __ret
	);
	ASSERT_OK(
		grv = gfmTilemap_newLoadf(
			tilemap
			, gameCtx
			, filename
			, len
			, tilemapTypeNames
			, tilemapTypes
			, numTilemapTypes
			, sidedCollisions
			, sidedCollisionsLen
		)
		, __ret
	);

	map->tilemaps[i] = tilemap;
	tilemap = 0;
	rv = 0;
__ret:
	if (tilemap) {
		gfmTilemap_free(&tilemap);
	}

	if (filename) {
		free(filename);
	}

	return rv;
}


/**
 * map_parseObjectEntry parses a single object into the list.
 *
 * This function also initializes the object list if it hadn't been yet.
 *
 * @param [out] objects: The list of objects.
 * @param [in] parser: The parser with the loaded objected.
 * @return 0: Success; Anything else: failure.
 */
static int map_parseObjectEntry(
	struct objectList *objects
	, gfmParser *parser
) {
	/** The object being parsed. */
	struct mapObject tmp = {0};
	/** The name of this object's type. */
	char *typeName;
	/** The type of the parsed entry. */
	gfmParserType type;
	/** How many attributes there are in this entry. */
	int num;
	/** Index for loops. */
	int i;
	/**  The return value (initialized to an error). */
	int rv = 1;
	gfmRV grv = GFMRV_OK;

	ASSERT_OK(grv = gfmParser_getType(&type, parser), __ret);

	/* If the list hasn't been initialized yet,
	 * ensure that the parser entry is the number of objects. */
	if (objects->count == 0) {
		/** The attribute's name. */
		char *key;
		/** The attribute's value. */
		char *val;

		/* Read the number of entries in this list. */
		ASSERT(type == gfmParserType_attributes, __ret);
		ASSERT_OK(grv = gfmParser_getNumProperties(&num, parser), __ret);
		ASSERT(num == 1, __ret);
		ASSERT_OK(grv = gfmParser_getProperty(&key, &val, parser, 0), __ret);
		ASSERT(0 == strcmp(key, "count"), __ret);
		ASSERT_OK(str2int(&objects->count, val), __ret);

		/* Allocate the object list. */
		ASSERT(
			objects->data = malloc(sizeof(struct mapObject) * objects->count)
			, __ret
		);
		memset(objects->data, 0, sizeof(struct mapObject) * objects->count);

		/* Finish the operation. */
		rv = 0;
		goto __ret;
	}

	/* Parse an object entry. */
	ASSERT(type == gfmParserType_object, __ret);
	ASSERT_OK(grv = gfmParser_getPos(&tmp.x, &tmp.y, parser), __ret);
	ASSERT_OK(
		grv = gfmParser_getDimensions(
			&tmp.width
			, &tmp.height
			, parser
		)
		, __ret
	);

	ASSERT_OK(grv = gfmParser_getIngameType(&typeName, parser), __ret);
	ASSERT_OK(type_get(&tmp.type, typeName, strlen(typeName)), __ret);

	/* Parse each attribute. */
	ASSERT_OK(grv = gfmParser_getNumProperties(&num, parser), __ret);
	if (num > 0) {
		ASSERT(
			tmp.attributes = (struct mapAttribute*)malloc(
				sizeof(struct mapAttribute) * num
			)
			, __ret
		);
		memset(tmp.attributes, 0, sizeof(struct mapAttribute) * num);
		tmp.numAttribute = num;
	}
	for (i = 0; i < num; i++) {
		/** The attribute's name. */
		char *key;
		/** The attribute's value. */
		char *val;
		/** The parsed attribute. */
		enum attribute attr;
		/** The type of the parsed attribute. */
		enum attrType type;
		/** The converted value. */
		int intValue;

		ASSERT_OK(grv = gfmParser_getProperty(&key, &val, parser, i), __ret);
		ASSERT_OK(attr_get(&attr, key, strlen(key)), __ret);
		attr_getType(&type, attr);

		/* Appropriately convert the value to an integer. */
		switch (type) {
		case ATTR_TYPE_INT:
			ASSERT_OK(str2int(&intValue, val), __ret);
			break;
		case ATTR_TYPE_STR:
			ASSERT_OK(store_addStr(&intValue, val, strlen(val)), __ret);
			break;
		default:
			ASSERT(0 /* Invalid attr type. */, __ret);
		}

		tmp.attributes[i].type = attr;
		tmp.attributes[i].value = intValue;
	}

	/* Store the parsed object. */
	for (i = 0; i < objects->count && objects->data[i].type != TYP_NONE; i++) {}
	ASSERT(i < objects->count, __ret);
	memcpy(objects->data + i, &tmp, sizeof(tmp));
	memset(&tmp, 0, sizeof(tmp));

	rv = 0;
__ret:
	map_freeObj(&tmp);

	return rv;
}


/**
 * map_parseObject parses a list of objects into the map.
 *
 * @param [out] map: The map that will receive the object list.
 * @param [in] meta: The metadata with the object list's information.
 * @param [in] mapDir: The map's directory.
 * @param [in] dirLen: dirLen's length.
 * @return 0: Success; Anything else: failure.
 */
static int map_parseObject(
	struct map *map
	, struct mapMetadata *meta
	, char *mapDir
	, int dirLen
) {
	/** Parser used to read the object file. */
	gfmParser *parser = 0;
	/** The recently loaded object list. */
	struct objectList tmp = {0};
	/** Index of the available object list where the loaded one will be stored. */
	int i;
	/**  The return value (initialized to an error). */
	int rv = 1;
	gfmRV grv = GFMRV_OK;

	/* Find and empty entry for the object. */
	for (i = 0; i < map->numObjectLists && map->objects[i].count > 0; i++) {}
	ASSERT(i < map->numObjectLists, __ret);

	/* Open the object file for parsing. */
	ASSERT_OK(grv = gfmParser_getNew(&parser), __ret);
	ASSERT_OK(
		map_openParser(
			parser
			, mapDir
			, dirLen
			, meta->file
			, strlen(meta->file)
		)
		, __ret
	);

	/* Parse each entry in the file. */
	do {
		gfmParserType type;

		grv = gfmParser_getType(&type, parser);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_PARSER_NO_OBJECT, __ret);
		if (grv == GFMRV_OK) {
			ASSERT_OK(map_parseObjectEntry(&tmp, parser), __ret);
		}

		grv = gfmParser_parseNext(parser);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_PARSER_FINISHED, __ret);
	} while (grv != GFMRV_PARSER_FINISHED);

	/* Store the entry in the map list. */
	memcpy(map->objects + i, &tmp, sizeof(tmp));
	memset(&tmp, 0, sizeof(tmp));

	rv = 0;
__ret:
	map_freeObjectList(&tmp);

	if (parser) {
		gfmParser_reset(parser);
		gfmParser_free(&parser);
	}

	return rv;
}


/**
 * map_parseMetaEntry parses a single metadata line into the map.
 *
 * @param [out] map: The map where the metadata will be parsed.
 * @param [in] parser: The parser at a new metadata entry.
 * @param [in] mapDir: The map's directory.
 * @param [in] dirLen: dirLen's length.
 * @return 0: Success; Anything else: failure.
 */
static int map_parseMetaEntry(
	struct map *map
	, gfmParser *parser
	, char *mapDir
	, int len
) {
	/** The type of the parsed entry. */
	gfmParserType type;
	/** The parsed metadata entry. */
	struct mapMetadata meta = {0};
	/** How many attributes there are in this entry. */
	int num;
	/**  The return value (initialized to an error). */
	int rv = 1;
	gfmRV grv = GFMRV_OK;

	/* Prepare to iterate over each property. */
	ASSERT_OK(grv = gfmParser_getType(&type, parser), __ret);
	ASSERT(type == gfmParserType_attributes, __ret);
	ASSERT_OK(grv = gfmParser_getNumProperties(&num, parser), __ret);

	/* Read every property into meta. */
	for (int i = 0; i < num; i++) {
		/** The attribute's name. */
		char *key;
		/** The attribute's value. */
		char *val;

		ASSERT_OK(grv = gfmParser_getProperty(&key, &val, parser, i), __ret);

		if (0 == strcmp(key, "name")) {
			meta.file = val;
		}
		else if (0 == strcmp(key, "type")) {
			if (0 == strcmp(val, "obj")) {
				meta.flags |= META_IS_OBJECT;
			}
			else if (0 == strcmp(val, "tilemap")) {
				meta.flags |= META_IS_TILEMAP;
			}
			else {
				ASSERT(0 /* Invalid type */, __ret);
			}
		}
		else if (0 == strcmp(key, "bg-color")) {
			ASSERT_OK(str2int(&meta.bgColor, val), __ret);
			meta.flags |= META_HAS_COLOR;
		}
		else if (0 == strcmp(key, "layer_count")) {
			ASSERT_OK(str2int(&meta.layerCount, val), __ret);
			meta.flags |= META_IS_COUNT;
		}
		else if (0 == strcmp(key, "obj_count")) {
			ASSERT_OK(str2int(&meta.objCount, val), __ret);
			meta.flags |= META_IS_COUNT;
		}
	}

	ASSERT(meta.flags, __ret);

	/* Allocate enough space in the map for each entry. */
	if (meta.flags & META_IS_COUNT) {
		ASSERT_OK(map_parseCount(map, &meta), __ret);
	}

	/* Update the map's BG color, if set. */
	if (meta.flags & META_HAS_COLOR) {
		map->bgColor = meta.bgColor;
	}

	/* Load the tilemap. */
	if (meta.flags & META_IS_TILEMAP) {
		ASSERT_OK(map_parseTilemap(map, &meta, mapDir, len), __ret);
	}

	/* Load the objects. */
	if (meta.flags & META_IS_OBJECT) {
		ASSERT_OK(map_parseObject(map, &meta, mapDir, len), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


int map_loadMap(struct map *map, char *mapDir, int len) {
	/** Parser used to read the metadata file. */
	gfmParser *parser = 0;
	/** Temporary map, where the parsed data is stored. */
	struct map tmp = {0};
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* Try to load the metadata file, to prepare the map. */
	ASSERT_OK(grv = gfmParser_getNew(&parser), __ret);
	ASSERT_OK(
		rv = map_openParserStaticFile(
			parser
			, mapDir
			, len
			, "meta.txt"
		)
		, __ret
	);

	do {
		gfmParserType type;

		grv = gfmParser_getType(&type, parser);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_PARSER_NO_OBJECT, __ret);
		if (grv == GFMRV_OK) {
			ASSERT_OK(
				rv = map_parseMetaEntry(&tmp, parser, mapDir, len)
				, __ret
			);
		}

		grv = gfmParser_parseNext(parser);
		ASSERT(grv == GFMRV_OK || grv == GFMRV_PARSER_FINISHED, __ret);
	} while (grv != GFMRV_PARSER_FINISHED);

	/* Store the parsed map. */
	memcpy(map, &tmp, sizeof(tmp));
	memset(&tmp, 0, sizeof(tmp));

	grv = GFMRV_OK;
__ret:
	map_free(&tmp);

	if (parser) {
		gfmParser_reset(parser);
		gfmParser_free(&parser);
	}

	return rv || grv;
}


void map_free(struct map *map) {
	int i;

	for (i = 0; i < map->numTilemaps; i++) {
		gfmTilemap_free(map->tilemaps + i);
	}
	free(map->tilemaps);
	map->tilemaps = 0;
	map->numTilemaps = 0;

	for (i = 0; i < map->numObjectLists; i++) {
		map_freeObjectList(map->objects + i);
	}
	free(map->objects);
	map->objects = 0;
	map->numObjectLists = 0;
}
