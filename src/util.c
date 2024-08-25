#include <error.h>
#include <util.h>

#include <limits.h>
#include <stdlib.h>


/**
 * str2long converts the given string to a long integer.
 *
 * This fails unless the entire string is a valid number!
 *
 * @param [out] num: The converted value.
 * @param [in] str: The input string.
 * @return 0: Success; Anything else: failure.
 */
static int str2long(long *num, const char *str) {
	/** Pointer to the end of the converted string. */
	char *end;
	/**  The return value (initialized to an error). */
	int rv = 1;

	*num = strtol(str, &end, 0);
	ASSERT(end && end != str && *end == '\0', __ret);
	ASSERT(*num != LONG_MIN && *num != LONG_MAX, __ret);

	rv = 0;
__ret:
	return rv;
}


int str2int(int *num, const char *str) {
	/** The converted value. */
	long value;
	/**  The return value (initialized to an error). */
	int rv = 1;

	ASSERT_OK(str2long(&value, str), __ret);
	ASSERT(value >= INT_MIN && value <= INT_MAX, __ret);

	*num = (int)value;

	rv = 0;
__ret:
	return rv;
}


int str2uint(unsigned int *num, const char *str) {
	/** The converted value. */
	long value;
	/**  The return value (initialized to an error). */
	int rv = 1;

	ASSERT_OK(str2long(&value, str), __ret);
	ASSERT(value >= 0 && value <= UINT_MAX, __ret);

	*num = (unsigned int)value;

	rv = 0;
__ret:
	return rv;
}
