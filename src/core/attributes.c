#include <core/attributes.h>
#include <error.h>

#include <string.h>


struct attrName {
	/** The name of the attribute. */
	const char * const name;
	/** The length of the name, assuming no terminating '\0'. */
	int len;
}


/** List of every available attribute name. */
static attrNames[ATTR_MAX] = {
#define X(attr, name, ...) \
	{ \
		name, \
		sizeof(name) - 1, \
	},
	ATTRIBUTES
#undef X
};


/** The type of each available attribute. */
static enum attrType attrTypes[ATTR_MAX] = {
#define X(attr, name, type) \
	type,
	ATTRIBUTES
#undef X
};


int attr_get(enum attribute *attr, char *name, int len) {
	int i;

	/* Remove the terminating '\0' from the name (if any). */
	while (len > 0 && name[len-1] == '\0') {
		len--;
	}

	for (i = 0; i < ATTR_MAX; i++) {
		struct attrName *got = attrNames + i;

		if (
			len == got->len &&
			0 == memcmp(got->name, name, len * sizeof(char))
		) {
			/** Add 1 for ATTR_NONE. */
			*attr = (enum attribute)(i + 1);
			return 0;
		}
	}

	return 1;
}


void attr_getType(enum attrType *type, enum attribute attr) {
	*type = attrTypes[attr];
}
