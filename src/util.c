#include <error.h>
#include <util.h>

#include <limits.h>
#include <math.h>
#include <stdint.h>
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


void normalize(double *x, double *y) {
	if (*x != 0.0 && *y != 0.0) {
		double delta = 1.0 / sqrt((*x) * (*x) + (*y) * (*y));

		*x *= delta;
		*y *= delta;
	}
}


void flinear_map(float *value, float zero, float one) {
	one -= zero;

	if (fabs(*value) <= zero) {
		*value = 0.0f;
	}
	else if (*value >= one) {
		*value = one;
	}
	else if (*value <= -one) {
		*value = -one;
	}
	else if (zero != 0.0f) {
		*value -= zero;
	}

	if (one != 1.0f) {
		*value /= one;
	}
}


void clampAbs(double *value, double max) {
	if (llabs(*value) > max) {
		if (*value > 0) {
			*value = max;
		}
		else {
			*value = -max;
		}
	}
}


/* Source: https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable */
static const uint8_t bitCountLookupTable[256] =
{
#	define B2(n) n,     n+1,     n+1,     n+2
#	define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#	define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
	B6(0), B6(1), B6(1), B6(2)
#	undef B2
#	undef B4
#	undef B6
};


int countBits(uint32_t value) {
	return bitCountLookupTable[value & 0xff]
		+ bitCountLookupTable[(value >> 8) & 0xff]
		+ bitCountLookupTable[(value >> 16) & 0xff]
		+ bitCountLookupTable[value >> 24];
}


static int seed = 0;


void util_setPRNGSeed(int newSeed) {
	seed = newSeed;
}


int util_prng() {
	long int tmp = seed;

	/* TODO */
	tmp *= 0x19660d;
	tmp += 0x3c6ef35f;
	seed = tmp;

	return seed;
}
