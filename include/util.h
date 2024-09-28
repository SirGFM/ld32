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
