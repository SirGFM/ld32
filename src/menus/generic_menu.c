#include <error.h>
#include <core/assets.h>
#include <menus/generic_menu.h>
#include <menus/mainmenu.h>


/** First index for the active text tiles. */
#define ACTIVE_TILES 0
/** First index for the inactive text tiles. */
#define INACTIVE_TILES 64
/** First index for the disabled text tiles. */
#define DISABLED_TILES 128


int menu_getType(enum menu *type, enum attribute attr) {
	int rv = 1;

	switch (attr) {
	case ATTR_MAINMENU_OPTION:
		*type = MENU_MAINMENU;
		break;
	default:
		ASSERT(0 /* invalid menu attribute */, __ret);
		break;
	}

	rv = 0;
__ret:
	return rv;
}


int menu_getIdx(int *idx, enum menu type, char *tag, int len) {
	int rv = 1;

	switch (type) {
	case MENU_MAINMENU:
		return mainmenu_getIdx(idx, tag, len);
	default:
		ASSERT(0 /* invalid menu type */, __ret);
		break;
	}

	rv = 0;
__ret:
	return rv;
}


int menu_getText(char **text, int *len, enum menu type, int idx) {
	int rv = 1;

	switch (type){
	case MENU_MAINMENU:
		ASSERT_OK(mainmenu_getText(text, len, idx), __ret);
		break;
	default:
		ASSERT(0 /* invalid menu type */, __ret);
		break;
	}

	rv = 0;
__ret:
	return rv;
}


int menu_setText(gfmText *text, enum menu type, int idx, int isActive) {
	char *str;
	int len;
	int rv = 1;

	ASSERT_OK(menu_getText(&str, &len, type, idx), __ret);

	ASSERT(GFMRV_OK == gfmText_setText(text, str, len, 1 /*copy*/), __ret);
	if (isActive) {
		ASSERT(
			GFMRV_OK == gfmText_setSpriteset(
				text
				, gfx8x8
				, ACTIVE_TILES
			)
			, __ret
		);
	}
	else {
		ASSERT(
			GFMRV_OK == gfmText_setSpriteset(
				text
				, gfx8x8
				, INACTIVE_TILES
			)
			, __ret
		);
		ASSERT(GFMRV_OK == gfmText_forceFinish(text), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


int menu_accept(enum menu type, int idx) {
	int rv = 1;

	switch (type){
	case MENU_MAINMENU:
		ASSERT_OK(mainmenu_accept(idx), __ret);
		break;
	default:
		ASSERT(0 /* invalid menu type */, __ret);
		break;
	}

	rv = 0;
__ret:
	return rv;
}
