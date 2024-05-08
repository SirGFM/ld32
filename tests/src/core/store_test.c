#include <core/store.h>
#include <error.h>

#include <common_test.h>

#include <string.h>
#include <stdio.h>


/** List of dummy values to be added and fetched from the store. */
static const char *values[] = {
	"a",
	"b",
	"a",
	"twas brilig, and the slithy toves",
	"did gyre and gimble in the wabe",
	"all mimsy were the borogoves",
	"and the mome raths outgrabe",
};


/** The total number of entries in values. */
const int total = sizeof(values) / sizeof(char *);


static void cleanup(void *noop) {
	store_free();
}


static void _test_store(void *noop) {
	int i, j;

	for (i = 0; i < total; i++) {
		int len = strlen(values[i]);
		int id;

		TEST_CHECK_STOP(0 == store_addStr(&id, (char*)(values[i]), len));
		TEST_CHECK_STOP(id == i);

		for (j = 0; j < total; j++) {
			char *gotStr;
			int gotLen;

			if (j <= i) {
				len = strlen(values[j]);

				TEST_CHECK_STOP(0 == store_getStr(&gotStr, &gotLen, j));
				TEST_CHECK_STOP(len == gotLen);
				TEST_CHECK(0 == memcmp(gotStr, values[j], len));
			}
			else {
				TEST_CHECK_STOP(0 != store_getStr(&gotStr, &gotLen, j));
			}
		}
	}
}


void test_store(void) {
	logErrors = 0;
	run_subtest("", 0, _test_store, cleanup, cleanup);
	logErrors = 1;
}
