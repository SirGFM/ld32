#include <config/config.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/store.h>
#include <error.h>
#include <setup.h>

int main(int argc, char *argv[]) {
	int rv;

	ASSERT_OK(rv = setup_init(), __ret);
	ASSERT_OK(rv = core_runGame(), __ret);

__ret:
#ifndef EMCC
	/* Avoid unloading if running on the web,
	 * since the mainloop is called at a later time. */
	setup_free();
#endif /* EMCC */

	return rv;
}
