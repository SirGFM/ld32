#include <config/config.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/store.h>
#include <error.h>

int main(int argc, char *argv[]) {
	struct config cfg;
	int rv;

	ASSERT_OK(rv = config_load(&cfg), __ret);
	ASSERT_OK(rv = core_init(&cfg), __ret);
	ASSERT_OK(rv = assets_loadGfx(), __ret);

	ASSERT_OK(rv = core_runGame(), __ret);

__ret:
#ifndef EMCC
	/* Avoid unloading if running on the web,
	 * since the mainloop is called at a later time. */
	store_free();
	core_free();
#endif /* EMCC */

	return rv;
}
