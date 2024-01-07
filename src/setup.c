#include <config/config.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/store.h>
#include <error.h>

int setup_init() {
	struct config cfg;
	int rv;

	ASSERT_OK(rv = config_load(&cfg), __ret);
	ASSERT_OK(rv = core_init(&cfg), __ret);
	ASSERT_OK(rv = assets_loadGfx(), __ret);

__ret:
	return rv;
}

void setup_free() {
	store_free();
	core_free();
}
