#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define TEST_NO_MAIN
#include <acutest.h>


/**
 * TEST_CHECK_STOP checks if the statement is true-ish,
 * stopping the test on error.
 *
 * This simply returns from the current function,
 * ignoring any clean up!
 */
#define TEST_CHECK_STOP(stmt) \
	do { \
		if (!TEST_CHECK(stmt)) { \
			return; \
		} \
	} while (0)


/**
 * STR "converts" the provided static string to a "tuple" of (string, len).
 *
 * This is mainly intended to be used to pass static strings as parameters.
 */
#define STR(value) value, sizeof(value) - 1


typedef void(*test_fn)(void *ctx);


/**
 * run_subtest executes a subtest as part of another test,
 * optionally executing running a setup and/or cleanup for the test.
 *
 * Note that if provided,
 * cleanup is called regardless of whether or not the test was successful.
 *
 * 'ctx' is passed as is to every function,
 * so it should be initialized in setup and released in cleanup.
 *
 * WARN: Do not use TEST_ASSERT(),
 * as these macros completely abort the test,
 * skipping any provided cleanup.
 * Also, if the test doesn't have a cleanup,
 * there's little reason to call this function.
 *
 * @param [in] name: The of this subtest.
 * @param [in/out] ctx: Arbitrary data passed to every function.
 * @param [in] run: The actual subtest.
 * @param [in] setup: Optional function called before the subtest.
 * @param [in] cleanup: Optional function called after the subtest.
 */
extern void run_subtest(
	const char *name
	, void *ctx
	, test_fn run
	, test_fn setup
	, test_fn cleanup
);

#endif /* TEST_COMMON_H */
