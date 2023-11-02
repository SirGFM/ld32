#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

/**
 * An attribute's underlying type.
 */
enum attrType {
	ATTR_TYPE_INT,
	ATTR_TYPE_STR,
};


#define ATTRIBUTES \
	X(ATTR_ID, "id", ATTR_TYPE_INT)


/**
 * Identifiers for attributes.
 */
enum attribute {
	ATTR_NONE,
#define X(attr, ...) \
	attr,
	ATTRIBUTES
#undef X
	ATTR_MAX
};


/**
 * attr_get retrieves the identifier for the given attribute.
 *
 * @param [out] attr: The attribute identifier.
 * @param [in] name: The name of the attribute.
 * @param [in] len: The name's length.
 * @return 0: Success; Anything else: failure.
 */
int attr_get(enum attribute *attr, char *name, int len);


/**
 * attr_getType retrieves the undelying type for the given attribute.
 *
 * @param [out] type: The attribute's undelying type.
 * @param [in] attr: The attribute identifier.
 */
void attr_getType(enum attrType *type, enum attribute attr);


#endif /* ATTRIBUTE_H */
