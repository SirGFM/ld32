#include <common_test.h>

extern void run_subtest(
	const char *name
	, void *ctx
	, test_fn run
	, test_fn setup
	, test_fn cleanup
) {
	TEST_CASE(name);

	if (setup) {
		setup(ctx);
	}
	run(ctx);
	if (cleanup) {
		cleanup(ctx);
	}

	TEST_CASE(0);
}
