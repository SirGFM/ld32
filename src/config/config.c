#include <GFraMe/gframe.h>
#include <GFraMe/gfmSave.h>

#include <config/config.h>

#include <core/core.h>
#include <error.h>


/**
 * config_getDefault returns a fully initialized config.
 *
 * @return A initialized struct config.
 */
static struct config config_getDefault() {
	struct config cfg = {
		.videoBackend = GFM_VIDEO_SDL2,
		.audioMode = gfmAudio_defQuality,
		.vsync = 0,
		.winWidth = VWIDTH * 2,
		.winHeight = VHEIGHT * 2,
		.updateRate = DEFAULT_FPS,
		.drawRate = DEFAULT_DPS,
		.wasLoaded = 0,
	};

	return cfg;
}


/**
 * config_loadFromFile loads a configuration from the supplied file.
 *
 * @param [out] cfg: The loaded configuration.
 * @param [in] pSave: The file with the saved configuration.
 * @return 0: Success; Anything else: failure.
 */
static int config_loadFromFile(struct config *cfg, gfmSave *pSave) {
	/* TODO */
	return 1;
}


int config_load(struct config *cfg) {
	/** The loaded config file (if any). */
	gfmSave *pSave = 0;
	/** Whether core should be unloaded. */
	int unload = 0;
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* If the game hasn't been initialized yet, temporarily initialize it. */
	if (!gameCtx) {
		struct config tmp;

		tmp = config_getDefault();
		ASSERT_OK(rv = core_initWindowless(&tmp), __ret);
		unload = 1;
	}

	/* Check if there's a config file,
	 * loading the default values otherwise. */
	ASSERT_OK(grv = gfmSave_getNew(&pSave), __ret);
	ASSERT_OK(grv = gfmSave_bindStatic(pSave, gameCtx, CONFIG_FILE), __ret);

	grv = gfmSave_findIdStatic(pSave, "saved");
	if (grv == GFMRV_OK) {
		ASSERT_OK(rv = config_loadFromFile(cfg, pSave), __ret);
	}
	else {
		*cfg = config_getDefault();
	}

	rv = 0;
	grv = GFMRV_OK;
__ret:
	if (pSave) {
		gfmSave_close(pSave);
		gfmSave_free(&pSave);
	}

	if (unload) {
		int tmp = core_free();
		rv = rv || tmp;
	}

	return rv || grv;
}
