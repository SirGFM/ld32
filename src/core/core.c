#include <GFraMe/gframe.h>
#ifdef EMCC
#	include <emscripten/emscripten.h>
#endif /* EMCC */

#include <core/core.h>
#include <error.h>
#include <mainloop.h>


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
	ASSERT_OK(grv = gfm_initFPSCounter(pCtx, 0, 0), __ret);

	gameCtx = pCtx;
	pCtx = 0;
__ret:
	if (pCtx) {
		grv = gfm_free(&pCtx);
	}

	return rv || grv;
}


/**
 * Configure core_runLoop as returning only when running natively,
 * as EMCC doesn't expect it to return anything.
 */
#ifdef EMCC
#	define	LOOP_RET void
#else
#	define	LOOP_RET int
#endif

/**
 * core_runLoop handles executing the game's actual loop.
 */
static LOOP_RET core_runLoop() {
	/**  The return value. */
	int rv;
	gfmRV grv = GFMRV_OK;

	/* If on HTML5, manually issue a frame. */
#ifdef EMCC
	ASSERT_OK(grv = gfm_issueFrame(gameCtx), __ret);
#endif /* EMCC */

	/* Wait for an event */
	ASSERT_OK(grv = gfm_handleEvents(gameCtx), __ret);

	while (gfm_isUpdating(gameCtx) == GFMRV_TRUE) {
		ASSERT_OK(grv = gfm_fpsCounterUpdateBegin(gameCtx), __ret);
		ASSERT_OK(rv = mainloop_update(), __ret);
		ASSERT_OK(grv = gfm_fpsCounterUpdateEnd(gameCtx), __ret);
	}

	while (gfm_isDrawing(gameCtx) == GFMRV_TRUE) {
		ASSERT_OK(grv = gfm_drawBegin(gameCtx), __ret);

		ASSERT_OK(rv = mainloop_draw(), __ret);

		ASSERT_OK(
			grv = gfm_drawRenderInfo(
				gameCtx
				, 0 /* ignored */
				, 0 /* x */
				, 24 /* y */
				, 0 /* ignored */
			)
			, __ret
		);
		ASSERT_OK(grv = gfm_drawEnd(gameCtx), __ret);
	}

	/* If on HTML5, manually sleep until the next frame. */
#ifdef EMCC
	ASSERT_OK(grv = gfm_waitFrame(gameCtx), __ret);
#endif /* EMCC */

__ret:
#ifndef EMCC
	return rv || grv;
#else
	/* On EMCC, add a noop so the label above may be valid. */
	do {} while (0);
#endif /* EMCC */
}


int core_runGame() {
	/** Whether the game should be unloaded. */
	int unload = 0;
	/**  The return value. */
	int rv;

	ASSERT_OK(rv = mainloop_init(), __ret);
#ifndef EMCC
	/* Avoid unloading if running on the web,
	 * since the mainloop is called at a later time. */
	unload = 1;
#endif /* EMCC */

#ifndef EMCC
	while (gfm_didGetQuitFlag(gameCtx) != GFMRV_TRUE) {
		ASSERT_OK(rv = core_runLoop(), __ret);
	}
#else /* ifdef EMCC */
	emscripten_set_main_loop((em_callback_func)core_runLoop, 0, 0);
#endif /* EMCC */

__ret:
	if (unload) {
		int tmp = mainloop_free();
		rv = rv || tmp;
	}

	return rv;
}
