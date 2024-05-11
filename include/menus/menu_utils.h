#ifndef MENU_UTILS_H
#define MENU_UTILS_H


/** A tag that uniquely identify an option in a menu. */
struct tag {
	/** The tag's string value. */
	const char *value;
	/** The value's length (without the terminating '\0'). */
	int len;
};


/**
 * STATIC_TAG converts VALUE into a statically defined struct tag.
 */
#define STATIC_TAG(VALUE) \
	{ \
		.value = VALUE, \
		.len = (sizeof(VALUE) - 1), \
	}

#endif /* MENU_UTILS_H */
