#include <setup.h>

#include <stdlib.h>

#define TEST_INIT {if (setup_init() != 0) { abort(); }}
#define TEST_FINI setup_free()

#include <acutest.h>

TEST_LIST = {
	{NULL, NULL}
};
