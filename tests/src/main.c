#include <setup.h>

#include <core/map_test.h>

#include <stdlib.h>

#define TEST_INIT {if (setup_init() != 0) { abort(); }}
#define TEST_FINI setup_free()

#include <acutest.h>

TEST_LIST = {
	{"map", test_map}
	, {NULL, NULL}
};
