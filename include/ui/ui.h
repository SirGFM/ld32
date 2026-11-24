#ifndef UI_H
#define UI_H

#include <scene.h>

/**
 * ui_init initializes the global UI group.
 *
 * @return 0: Success; Anything else: failure.
 */
int ui_init();

/**
 * ui_free releases all resources associated with the global UI group.
 */
void ui_free();

/**
 * ui_update updates every UI element.
 *
 * @return 0: Success; Anything else: failure.
 */
int ui_update(struct scene *scene);

/**
 * ui_draw draws every UI element.
 *
 * @return 0: Success; Anything else: failure.
 */
int ui_draw(struct scene *scene);

#endif /* UI_H */
