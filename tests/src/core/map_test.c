#include <core/attributes.h>
#include <core/map.h>
#include <core/types.h>
#include <error.h>

#include <common_test.h>

#include <string.h>


static void setup(void *ctx) {
	struct map *map = (struct map*)ctx;

	memset(map, 0x0, sizeof(struct map));
}


static void cleanup(void *ctx) {
	struct map *map = (struct map*)ctx;

	map_free(map);
	memset(map, 0x0, sizeof(struct map));
}


#define TEST_FUNC(name) \
	static void test_ ## name(void *ctx)


#define RUN(name, ctx) \
	do { \
		run_subtest(#name, ctx, test_ ## name, setup, cleanup); \
	} while (0)


TEST_FUNC(missing_meta) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 != map_loadMap(map, STR("nope")));
}


TEST_FUNC(missing_obj) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 != map_loadMap(map, STR("maps/test/no_obj")));
}


TEST_FUNC(missing_tilemap) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 != map_loadMap(map, STR("maps/test/no_tilemap")));
}


TEST_FUNC(invalid_obj_type) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 != map_loadMap(map, STR("maps/test/invalid_obj_type")));
}


TEST_FUNC(invalid_obj_attr_type) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 != map_loadMap(map, STR("maps/test/invalid_obj_attr_type")));
}

TEST_FUNC(parse_single_obj) {
	struct map *map = (struct map*)ctx;

	TEST_CHECK_STOP(0 == map_loadMap(map, STR("maps/test/single_obj")));

	TEST_CHECK(map->numTilemaps == 0);
	TEST_CHECK_STOP(map->numObjectLists == 1);

	TEST_CHECK_STOP(map->objects[0].count == 1);
	TEST_CHECK(map->objects[0].data[0].type == TYP_PLAYER);
	TEST_CHECK(map->objects[0].data[0].x == 1);
	TEST_CHECK(map->objects[0].data[0].y == 23);
	TEST_CHECK(map->objects[0].data[0].width == 456);
	TEST_CHECK(map->objects[0].data[0].height == 89);

	TEST_CHECK_STOP(map->objects[0].data[0].numAttribute == 3);
	TEST_CHECK(map->objects[0].data[0].attributes[0].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[0].value == 1234);
	TEST_CHECK(map->objects[0].data[0].attributes[1].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[1].value == 5);
	TEST_CHECK(map->objects[0].data[0].attributes[2].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[2].value == 67);
}


/**
 * check_area verifies if a given area within a tilemap is as expected.
 *
 * @param [in] map: The map being tested.
 * @param [in] i: The area's index.
 * @param [in] x: The area's horizontal position in tiles.
 * @param [in] y: The area's vertical position in tiles.
 * @param [in] w: The area's width in tiles.
 * @param [in] h: The area's height in tiles.
 * @param [in] t: The area's underlying type.
 */
static void check_area(gfmTilemap *map, int i, int x, int y, int w, int h, enum type t) {
	gfmObject *obj;
	void *tmp;
	int a, b;

	TEST_CHECK_STOP(0 == gfmTilemap_getArea(&obj, map, i));
	TEST_CHECK_STOP(0 == gfmObject_getDimensions(&a, &b, obj));
	TEST_CHECK(a == w * 8);
	TEST_CHECK(b == h * 8);
	TEST_CHECK_STOP(0 == gfmObject_getPosition(&a, &b, obj));
	TEST_CHECK(a == x * 8);
	TEST_CHECK(b == y * 8);
	TEST_CHECK_STOP(0 == gfmObject_getChild(&tmp, &a, obj));
	TEST_CHECK(a == t);
}


TEST_FUNC(parse_single_tilemap) {
	struct map *map = (struct map*)ctx;
	int w, h, num;

	TEST_CHECK_STOP(0 == map_loadMap(map, STR("maps/test/single_map")));

	TEST_CHECK_STOP(map->numTilemaps == 1);
	TEST_CHECK_STOP(map->numObjectLists == 0);
	TEST_CHECK(map->bgColor == 0x12345678);

	TEST_CHECK_STOP(map->tilemaps[0] != NULL);
	TEST_CHECK_STOP(0 == gfmTilemap_getDimension(&w, &h, map->tilemaps[0]));
	TEST_CHECK(w == 80);
	TEST_CHECK(h == 40);

	TEST_CHECK_STOP(0 == gfmTilemap_getAreasLength(&num, map->tilemaps[0]));
	TEST_CHECK_STOP(num == 9);

	TEST_CASE("parse_single_tilemap: area 0");
	check_area(map->tilemaps[0], 0, 0, 0, 5, 1, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 1");
	check_area(map->tilemaps[0], 1, 4, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 2");
	check_area(map->tilemaps[0], 2, 0, 2, 5, 1, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 3");
	check_area(map->tilemaps[0], 3, 0, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 4");
	check_area(map->tilemaps[0], 4, 7, 2, 2, 1, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 5");
	check_area(map->tilemaps[0], 5, 8, 2, 1, 2, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 6");
	check_area(map->tilemaps[0], 6, 7, 3, 2, 1, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 7");
	check_area(map->tilemaps[0], 7, 7, 2, 1, 2, TYP_FLOOR);
	TEST_CASE("parse_single_tilemap: area 8");
	check_area(map->tilemaps[0], 8, 1, 4, 8, 1, TYP_SPIKES);
}


TEST_FUNC(parse_complex_map) {
	struct map *map = (struct map*)ctx;
	int w, h, num;

	TEST_CHECK_STOP(0 == map_loadMap(map, STR("maps/test/complex_map")));

	TEST_CHECK_STOP(map->numObjectLists == 1);
	TEST_CHECK_STOP(map->numTilemaps == 2);
	TEST_CHECK(map->bgColor == 0x12345678);

	/* obj.txt */
	TEST_CASE("parse_complex_tilemap: obj.txt");
	TEST_CHECK_STOP(map->objects[0].count == 1);
	TEST_CHECK(map->objects[0].data[0].type == TYP_PLAYER);
	TEST_CHECK(map->objects[0].data[0].x == 1);
	TEST_CHECK(map->objects[0].data[0].y == 23);
	TEST_CHECK(map->objects[0].data[0].width == 456);
	TEST_CHECK(map->objects[0].data[0].height == 89);

	TEST_CHECK_STOP(map->objects[0].data[0].numAttribute == 3);
	TEST_CHECK(map->objects[0].data[0].attributes[0].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[0].value == 1234);
	TEST_CHECK(map->objects[0].data[0].attributes[1].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[1].value == 5);
	TEST_CHECK(map->objects[0].data[0].attributes[2].type == ATTR_ID);
	TEST_CHECK(map->objects[0].data[0].attributes[2].value == 67);

	/* bg.txt */
	TEST_CASE("parse_complex_tilemap: bg.txt");
	TEST_CHECK_STOP(map->tilemaps[0] != NULL);
	TEST_CHECK_STOP(0 == gfmTilemap_getDimension(&w, &h, map->tilemaps[0]));
	TEST_CHECK(w == 80);
	TEST_CHECK(h == 40);

	TEST_CHECK_STOP(0 == gfmTilemap_getAreasLength(&num, map->tilemaps[0]));
	TEST_CHECK_STOP(num == 9);

	TEST_CASE("parse_complex_tilemap[0]: area 0");
	check_area(map->tilemaps[0], 0, 0, 0, 5, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 1");
	check_area(map->tilemaps[0], 1, 4, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 2");
	check_area(map->tilemaps[0], 2, 0, 2, 5, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 3");
	check_area(map->tilemaps[0], 3, 0, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 4");
	check_area(map->tilemaps[0], 4, 7, 2, 2, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 5");
	check_area(map->tilemaps[0], 5, 8, 2, 1, 2, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 6");
	check_area(map->tilemaps[0], 6, 7, 3, 2, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 7");
	check_area(map->tilemaps[0], 7, 7, 2, 1, 2, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[0]: area 8");
	check_area(map->tilemaps[0], 8, 1, 4, 8, 1, TYP_SPIKES);

	/* fg.txt */
	TEST_CASE("parse_complex_tilemap: fg.txt");
	TEST_CHECK_STOP(map->tilemaps[1] != NULL);
	TEST_CHECK_STOP(0 == gfmTilemap_getDimension(&w, &h, map->tilemaps[1]));
	TEST_CHECK(w == 64);
	TEST_CHECK(h == 32);

	TEST_CHECK_STOP(0 == gfmTilemap_getAreasLength(&num, map->tilemaps[1]));
	TEST_CHECK_STOP(num == 5);

	TEST_CASE("parse_complex_tilemap[1]: area 0");
	check_area(map->tilemaps[1], 0, 1, 0, 4, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[1]: area 1");
	check_area(map->tilemaps[1], 1, 4, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[1]: area 2");
	check_area(map->tilemaps[1], 2, 1, 2, 4, 1, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[1]: area 3");
	check_area(map->tilemaps[1], 3, 1, 0, 1, 3, TYP_FLOOR);
	TEST_CASE("parse_complex_tilemap[1]: area 4");
	check_area(map->tilemaps[1], 4, 1, 3, 6, 1, TYP_SPIKES);
}


void test_map(void) {
	struct map map;

	/* Disable logging for sanity tests. */
	logErrors = 0;
	RUN(missing_meta, &map);
	RUN(missing_obj, &map);
	RUN(missing_tilemap, &map);
	RUN(invalid_obj_type, &map);
	RUN(invalid_obj_attr_type, &map);

	logErrors = 1;
	RUN(parse_single_obj, &map);
	RUN(parse_single_tilemap, &map);
	RUN(parse_complex_map, &map);
}
