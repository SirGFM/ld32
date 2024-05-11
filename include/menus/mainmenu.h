#ifndef MAINMENU_H
#define MAINMENU_H

/**
 * mainmenu_getIdx retrieves the index of the option in the scene.
 *
 * @param [out] idx: The option's index.
 * @param [in] tag: A unique textual identifier within the mainmenu.
 * @param [in] len: The tag's length.
 * @return 0: Success; Anything else: failure.
 */
int mainmenu_getIdx(int *idx, char *tag, int len);


/**
 * mainmenu_getText retrieves the text for the option at that given index.
 *
 * @param [out] text: The option's text.
 * @param [out] len: The text's length.
 * @param [in] idx: The option's index.
 * @return 0: Success; Anything else: failure.
 */
int mainmenu_getText(char **text, int *len, int idx);


#endif /* MAINMENU_H */
