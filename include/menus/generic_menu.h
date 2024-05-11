#ifndef GENERIC_MENU_H
#define GENERIC_MENU_H


#include <core/attributes.h>

#include <GFraMe/gfmSpriteset.h>
#include <GFraMe/gfmText.h>


/** The list of available menus (that use these options). */
enum menu {
	  MENU_MAINMENU = 0
	, MENU_MAX
};


/**
 * menu_getType retrieves the menu type in which a specific attribute is expected.
 *
 * @param [out] type: The menu type.
 * @param [in] attr: The attribute.
 * @return 0: Success; Anything else: failure.
 */
int menu_getType(enum menu *type, enum attribute attr);


/**
 * menu_getIdx retrieves the index of the option in the scene.
 *
 * @param [out] idx: The option's index.
 * @param [in] type: The menu type.
 * @param [in] tag: A unique textual identifier within the menu type.
 * @param [in] len: The tag's length.
 * @return 0: Success; Anything else: failure.
 */
int menu_getIdx(int *idx, enum menu type, char *tag, int len);


/**
 * menu_getText retrieves the text for the option menu type.
 *
 * @param [out] text: The option's text.
 * @param [out] len: The text's length.
 * @param [in] type: The menu type.
 * @param [in] idx: The option's index.
 * @return 0: Success; Anything else: failure.
 */
int menu_getText(char **text, int *len, enum menu type, int idx);


/**
 * menu_setText updates the text.
 *
 * If a text is active, it shall appear one letter at a time.
 * Otherwise, it appears all at once.
 *
 * @param [in] text: The gfmText being updated.
 * @param [in] type: The menu type.
 * @param [in] idx: The option's index.
 * @param [in] isActive: Whether this text is currently active.
 * @return 0: Success; Anything else: failure.
 */
int menu_setText(gfmText *text, enum menu type, int idx, int isActive);


#endif /* GENERIC_MENU_H */
