#include <GFraMe/gframe.h>

#include <core/assets.h>
#include <core/core.h>
#include <error.h>


gfmSpriteset *gfx8x8;
gfmSpriteset *gfx16x16;


int assets_loadGfx() {
	/** Index of the loaded texture. */
	int texture;
	/**  The return value. */
	int rv = 0;
	gfmRV grv = GFMRV_OK;

	if (gfx8x8 || gfx16x16) {
		rv = 1;
		ASSERT(1 /* already initialized! */, __ret);
	}

	ASSERT_OK(
		grv = gfm_loadTextureStatic(
			&texture
			, gameCtx
			, TEXTURE_ATLAS
			, TEXTURE_CHROMA_KEY
		)
		, __ret
	);

	ASSERT_OK(
		grv = gfm_createSpritesetCached(
			&gfx8x8
			, gameCtx
			, texture
			, 8
			, 8
		)
		, __ret
	);

	ASSERT_OK(
		grv = gfm_createSpritesetCached(
			&gfx16x16
			, gameCtx
			, texture
			, 16
			, 16
		)
		, __ret
	);

__ret:
	return rv || grv;
}
