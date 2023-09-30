#include <config/config.h>
#include <core/core.h>
#include <error.h>

int main(int argc, char *argv[]) {
	struct config cfg;
	int rv;

	ASSERT_OK(rv = config_load(&cfg), __ret);
	ASSERT_OK(rv = core_init(&cfg), __ret);

__ret:
	core_free();

	return rv;
}
