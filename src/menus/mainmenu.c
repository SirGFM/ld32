#include <error.h>
#include <menus/mainmenu.h>
#include <menus/menu_utils.h>

#include <string.h>

enum mainmenu_options {
	  MM_FILE_1 = 0
	, MM_FILE_2
	, MM_FILE_3
	, MM_OPTIONS
	, MM_EXIT
	, MM_MAX
};


/** The main menu options sorted as they should appear. */
static const struct tag sortedTags[MM_MAX] = {
	[MM_FILE_1] = STATIC_TAG("file_1"),
	[MM_FILE_2] = STATIC_TAG("file_2"),
	[MM_FILE_3] = STATIC_TAG("file_3"),
	[MM_OPTIONS] = STATIC_TAG("options"),
	[MM_EXIT] = STATIC_TAG("exit"),
};


/** The default main menu text for each optio. */
static const struct tag defaultText[MM_MAX] = {
	[MM_FILE_1] = STATIC_TAG("FILE A: NEW GAME"),
	[MM_FILE_2] = STATIC_TAG("FILE B: NEW GAME"),
	[MM_FILE_3] = STATIC_TAG("FILE C: NEW GAME"),
	[MM_OPTIONS] = STATIC_TAG("OPTIONS"),
	[MM_EXIT] = STATIC_TAG("EXIT"),
};

/* TODO: Add buffers for continue text. */


int mainmenu_getIdx(int *idx, char *tag, int len) {
	int i;

	for (i = 0; i < MM_MAX; i++) {
		if (sortedTags[i].len != len) {
			continue;
		}

		if (0 == memcmp(sortedTags[i].value, tag, len)) {
			*idx = i;
			return 0;
		}
	}
	ASSERT(0 /* invalid mainmenu tag. */, __ret);

__ret:
	return 1;
}


/**
 * _mainmenu_getFileText gets the appropriate text for the given file.
 *
 * If the slot was started, it generates a dynamic "continue" text for the option.
 * Otherwise, the default text is returned.
 *
 * @param [out] text: The option's text.
 * @param [out] len: The text's length.
 * @param [in] idx: The option's index.
 * @return 0: Success; Anything else: failure.
 */
static int mainmenu_getFileText(char **text, int *len, enum mainmenu_options idx) {
	/* TODO: Implement _mainmenu_getContinueText. */

	*text = (char *)defaultText[idx].value;
	*len = defaultText[idx].len;

	return 0;
}


int mainmenu_getText(char **text, int *len, int idx) {
	int rv = 1;

	ASSERT(idx >= 0 && idx < MM_MAX, __ret);

	switch (idx) {
	case MM_FILE_1:
	case MM_FILE_2:
	case MM_FILE_3:
		ASSERT_OK(mainmenu_getFileText(text, len, idx), __ret);
		break;
	case MM_OPTIONS:
	case MM_EXIT:
		*text = (char *)defaultText[idx].value;
		*len = defaultText[idx].len;
		break;
	}

	rv = 0;
__ret:
	return rv;
}
