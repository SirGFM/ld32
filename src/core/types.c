#include "core/types.h"

#include <string.h>


char *tilemapTypeNames[] = {
#define TM_SIDED(typ, name, ...) \
	name,
#define TM(typ, name, ...) \
	name,
#define X(...)
	TYPES
#undef X
#undef TM
#undef TM_SIDED
};

int tilemapTypes[] = {
#define TM_SIDED(typ, ...) \
	(int)typ,
#define TM(typ, ...) \
	(int)typ,
#define X(...)
	TYPES
#undef X
#undef TM
#undef TM_SIDED
};

int numTilemapTypes = sizeof(tilemapTypes) / sizeof(int);

int sidedCollisions[] = {
#define TM_SIDED(typ, ...) \
	(int)typ,
#define TM(...)
#define X(...)
	TYPES
#undef X
#undef TM
#undef TM_SIDED
};

int sidedCollisionsLen = sizeof(sidedCollisions) / sizeof(int);


struct typeName {
	/** The name of the type. */
	const char const *name;
	/** The length of the name, assuming no terminating '\0'. */
	int len;
	/** The underlying type. */
	enum type type;
};


/** List with every name for every type. */
struct typeName fullTypeNames[] = {
#define TM_SIDED(typ, name, ...) \
	{ \
		name, \
		sizeof(name) - 1, \
		typ, \
	},
#define TM(typ, name, ...) \
	{ \
		name, \
		sizeof(name) - 1, \
		typ, \
	},
#define X(typ, name, ...) \
	{ \
		name, \
		sizeof(name) - 1, \
		typ, \
	},
	TYPES
#undef X
#undef TM
#undef TM_SIDED
};


/** The number of differents types. */
#define TYP_MAX (sizeof(fullTypeNames) / sizeof(struct typeName))


int type_get(enum type *type, char *name, int len) {
	int i;

	/* Remove the terminating '\0' from the name (if any). */
	while (len > 0 && name[len-1] == '\0') {
		len--;
	}

	for (i = 0; i < TYP_MAX; i++) {
		struct typeName *got = fullTypeNames + i;

		if (
			len == got->len &&
			0 == memcmp(got->name, name, len * sizeof(char))
		) {
			*type = got->type;
			return 0;
		}
	}

	return 1;
}
