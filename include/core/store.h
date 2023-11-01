#ifndef STORE_H
#define STORE_H


/**
 * store_free releases the storage.
 */
void store_free();


/**
 * store_addStr stores a string in the storage.
 *
 * @param [out] id: The identifier used to later retrieve this value.
 * @param [in] str: The string to be stored.
 * @param [in] len: The length of the string.
 * @return 0: Success; Anything else: failure.
 */
int store_addStr(int *id, char *str, int len);


/**
 * store_getStr retrieves a previously stored string from the storage.
 *
 * The returned value shouldn't be modified.
 *
 * @param [out] str: The string.
 * @param [out] len: The length of the string.
 * @param [in] id: The identifier obtained when storing the string.
 * @return 0: Success; Anything else: failure.
 */
int store_getStr(char **str, int *len, int id);


#endif /* STORE_H */
