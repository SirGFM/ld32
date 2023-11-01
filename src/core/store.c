#include <core/store.h>
#include <error.h>

#include <stdlib.h>
#include <string.h>


/**
 * LIST declares a new list type.
 *
 * @param [in] listName: The name of the created type.
 * @param [in] type: The type of the items in the list.
 */
#define LIST(listName, type) \
	typedef struct { \
		/** The list of entries. */ \
		type *list; \
		/** The number of entries stored in the list. */ \
		int len; \
		/** The number of slots in the list. */\
		int cap; \
	} listName


struct str {
	/** The stored string. */
	char *data;
	/** The string's length. */
	int len;
};


LIST(strList, struct str);


struct storage {
	/** The string storage. */
	strList str;
};


/** The store where everything is saved. */
static struct storage store;


void store_free() {
	int i;

	/** Release every stored string. */
	for (i = 0; i < store.str.len; i++) {
		free(store.str.list[i].data);
	}
	if (store.str.list) {
		free(store.str.list[i].data);
	}
	memset(&store.str, 0, sizeof(store.str));
}


int store_addStr(int *id, char *src, int len) {
	/** The region where the string will be stored. */
	struct str *dst;
	/** The return value (initialized to an error). */
	int rv = 1;

	/* If the list is full, double its capacity. */
	if (store.str.len == store.str.cap) {
		/** The new list. */
		void *new;
		/** The new capacity of the list. */
		int cap = 1 + store.str.cap * 2;

		ASSERT(new = realloc(store.str.list, sizeof(struct str)), __ret);
		memset(store.str.list, 0, (cap - store.str.cap) * sizeof(struct str));
		store.str.list = new;
		store.str.cap = cap;
	}

	/* Allocate the string's storage and store it. */
	dst = store.str.list + store.str.len;
	ASSERT(dst->data = malloc(sizeof(char) * len), __ret);
	memcpy(dst->data, src, sizeof(char) * len);
	dst->len = len;

	/* Return the identifier and finish adding the string to the list. */
	*id = store.str.len;
	store.str.len++;

	rv = 0;
__ret:
	return rv;
}


int store_getStr(char **str, int *len, int id) {
	/** The region where the string was stored. */
	struct str *src;
	/** The return value (initialized to an error). */
	int rv = 1;

	/* Ensure that the identifier is in the list. */
	ASSERT(id < store.str.len, __ret);

	/* Return the string and its length. */
	*str = store.str.list[id].data;
	*len = store.str.list[id].len;

	rv = 0;
__ret:
	return rv;
}
