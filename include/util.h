#ifndef UTIL_H
#define UTIL_H

/**
 * str2int converts the given string to an integer.
 *
 * This fails unless the entire string is a valid number!
 *
 * @param [out] num: The converted value.
 * @param [in] str: The input string.
 * @return 0: Success; Anything else: failure.
 */
int str2int(int *num, const char *str);

/**
 * str2uint converts the given string to a unsigned integer.
 *
 * This fails unless the entire string is a valid number!
 *
 * @param [out] num: The converted value.
 * @param [in] str: The input string.
 * @return 0: Success; Anything else: failure.
 */
int str2uint(unsigned int *num, const char *str);


/**
 * normalize converts the provided 2D vector into a unit vector,
 * ensuring its length is 1.0.
 *
 * @param [out] x: The horizontal component of the vector.
 * @param [out] y: The vertical component of the vector.
 */
void normalize(double *x, double *y);


/**
 * flinear_map linearly maps the provided value to the provided range,
 * normalizing the result so it's in the range [0, 1].
 *
 * @param [out] value: The value being mapped.
 * @param [in] zero: The value that should be mapped to 0.
 * @param [in] one: The value that should be mapped to 1.
 */
void flinear_map(float *value, float zero, float one);


/**
 * clampAbs limits value so its absolute is max,
 * but maintaining its sign.
 *
 * @param [out] value: The value being converted.
 * @param [in] max: The maximum absolute value.
 */
void clampAbs(double *value, double max);


/**
 * min returns the smaller of the two values.
 *
 * Since this is a macro, becareful of issues caused by type conversion.
 *
 * @param [in] a: One of the values.
 * @param [in] b: The other value.
 * @return The smallests value.
 */
#define min(a, b) \
	((a) < (b) ? (a) : (b))


/**
 * max returns the greater of the two values.
 *
 * Since this is a macro, becareful of issues caused by type conversion.
 *
 * @param [in] a: One of the values.
 * @param [in] b: The other value.
 * @return The greatest value.
 */
#define max(a, b) \
	((a) > (b) ? (a) : (b))


#endif /* UTIL_H */
