#ifndef ERROR_H
#define ERROR_H


#ifdef DEBUG
/** Global variable used to disable logging (for example, in tests). */
extern int logErrors;
#endif


/**
 * LOG_STMT logs the current position and the supplied stmt.
 *
 * @param {stmt} - Whatever statement should be logged.
 */
#ifdef DEBUG
#	include <stdio.h>
#	define LOG_STMT(stmt) \
		do { \
			if (logErrors) { \
				printf( \
					"[%s() - %s @ %d] : " #stmt "\n" \
					, __FUNCTION__ \
					, __FILE__ \
					, __LINE__ \
				); \
			} \
		} while (0)
#else
#	define LOG_STMT(stmt) do {} while (0)
#endif


/**
 * ASSERT checks whether stmt is true,
 * jumping to label otherwise.
 *
 * @param {stmt} - The asserted statement, that should yield "true".
 * @param {label} - Label jumped-to on error.
 */
#define ASSERT(stmt, label) \
	do { \
		if (!(stmt)) { \
			LOG_STMT(stmt); \
			goto label; \
		} \
	} while (0)


/**
 * ASSERT_OK checks whether the value reports a successful operation,
 * jumping to label otherwise.
 *
 * @param {rv} - The value that should identify success (i.e., 0).
 * @param {label} - Label jumped-to on error.
 */
#define ASSERT_OK(rv, label) ASSERT(0 == (rv), label)


#endif /* ERROR_H */
