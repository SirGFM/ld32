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

#endif /* UTIL_H */
