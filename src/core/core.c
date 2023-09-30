#include <GFraMe/gframe.h>

#include <core/core.h>
#include <error.h>


gfmCtx *gameCtx;


int core_free() {
	gfmRV grv = GFMRV_OK;

	ASSERT(gameCtx, __ret);

	ASSERT_OK(grv = gfm_free(&gameCtx), __ret);
	gameCtx = 0;

__ret:
	return (int)grv;
}


int core_initWindowless(struct config *cfg) {
	/** The context being loaded. */
	gfmCtx *pCtx = 0;
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	if (gameCtx) {
		rv = 1;
		ASSERT(0 /* already initialized! */, __ret);
	}

	ASSERT_OK(grv = gfm_getNew(&pCtx), __ret);
	ASSERT_OK(grv = gfm_setVideoBackend(pCtx, cfg->videoBackend), __ret);
	ASSERT_OK(grv = gfm_initStatic(pCtx, ORG, TITLE), __ret);

	gameCtx = pCtx;
	pCtx = 0;
__ret:
	if (pCtx) {
		grv = gfm_free(&pCtx);
	}

	return rv || grv;
}


int core_init(struct config *cfg) {
	/** The context being loaded. */
	gfmCtx *pCtx = 0;
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	/* Do the initial loading, but clear the global state temporarily. */
	ASSERT_OK(rv = core_initWindowless(cfg), __ret);
	pCtx = gameCtx;
	gameCtx = 0;

	ASSERT_OK(
		grv = gfm_initGameWindow(
			pCtx
			, VWIDTH
			, VHEIGHT
			, cfg->winWidth
			, cfg->winHeight
			, 0 /* user-resizable */
			, cfg->vsync
		)
		, __ret
	);

	ASSERT_OK(grv = gfm_setFPS(pCtx, MAX_FPS), __ret);
	ASSERT_OK(
		grv = gfm_setStateFrameRate(
			pCtx
			, cfg->updateRate
			, cfg->drawRate
		)
		, __ret
	);
	ASSERT_OK(grv = gfm_initAudio(pCtx, cfg->audioMode), __ret);

	gameCtx = pCtx;
	pCtx = 0;
__ret:
	if (pCtx) {
		grv = gfm_free(&pCtx);
	}

	return rv || grv;
}
