#include <error.h>
#include <util.h>

#include <limits.h>
#include <stdlib.h>

int str2int(int *num, const char *str) {
	/** Pointer to the end of the converted string. */
	char *end;
	/** The converted value. */
	long value;
	/**  The return value (initialized to an error). */
	int rv = 1;

	value = strtol(str, &end, 0);
	ASSERT(end && end != str && *end == '\0', __ret);
	ASSERT(value != LONG_MIN && value != LONG_MAX, __ret);
	ASSERT(value >= INT_MIN && value <= INT_MAX, __ret);

	*num = (int)value;

	rv = 0;
__ret:
	return rv;
}
