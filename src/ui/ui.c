#include <core/core.h>
#include <error.h>
#include <scene.h>
#include <ui/hud.h>
#include <ui/ui.h>


int ui_init() {
	int rv = 1;

	ASSERT_OK(hud_init(), __ret);

	rv = 0;
__ret:
	return rv;
}


void ui_free() {
	hud_free();
}


int ui_update(struct scene *scene) {
	int rv = 1;

	hud_disable();

	rv = 0;
__ret:
	return rv;
}


int ui_draw(struct scene *scene) {
	int rv = 1;

	ASSERT_OK(hud_draw(scene), __ret);

	rv = 0;
__ret:
	return rv;
}
