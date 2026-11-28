#include <camera.h>
#include <core/assets.h>
#include <core/core.h>
#include <core/types.h>
#include <error.h>
#include <global.h>
#include <scene.h>

#include <GFraMe/gframe.h>
#include <GFraMe/gfmTilemap.h>


/** The HUD's width, in tiles. */
#define HUD_WIDTH_TILES 15

/** The HUD's height, in tiles. */
#define HUD_HEIGTH_TILES 4

/** Initial horizontal position of the fuel bar. */
#define HUD_BARS_X 26

/** Vertical position of the fuel bar. */
#define HUD_BARS_Y 0

/** Position of the tile slightly overlaying the fuel bar. */
#define HUD_BARS_OVERLAY_X 24

/** Tile used to slightly overlay the fuel bar. */
#define HUD_BARS_OVERLAY_TILE 4075

/** First tile of the bar. */
#define HUD_BARS_FIRST_TILE 4019

/** First tile of the slow reload bar. */
#define HUD_BARS_RELOAD_FIRST_TILE 4083

/** Distance between each bar. */
#define HUD_BARS_OFFSET 3

/** Number of bars per color. */
#define HUD_BAR_PER_COLOR 4

/** The HUD's static elements. */
static gfmTilemap *hudBG;

/** Whether the HUD is currently eneabled. */
static int enabled;

/** Total fuel (maxed bar). */
static int maxFuel;

/** Current fuel (empting bar). */
static int curFuel;

/** Whether the fuel was fully spent and is slowly recharging. */
static int slowBar;

/** The currently active stones. */
static enum rainbowColor active;

/** HUD's tiles. */
static int hudData[] = {
	3880,3881,3882,3883,3884,3885,3886,3887,3888,3889,3890,3891,3892,3893,3894,
	3944,3945,3946,3947,3948,3949,3950,3951,3952,3953,3954,3955,3956,3957,3958,
	4008,4009,4010,4011,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	4072,4073,4074,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
};

/** Structure for holding the stone look-up table. */
struct stone {
	/** Color represented by this stone. */
	enum rainbowColor color;
	/** Horizontal offset. */
	int x;
	/** Vertical offset. */
	int y;
	/** Tiled used to represent the collected and active stone. */
	int activeTile;
	/** Tiled used to represent the collected, but inactive, stone. */
	int collectedTile;
};

/** The stones look-up table. */
struct stone stonesLUT[];


int hud_init() {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmTilemap_getNew(&hudBG), __ret);
	ASSERT(GFMRV_OK == gfmTilemap_init(hudBG, gfx8x8, HUD_WIDTH_TILES, HUD_HEIGTH_TILES, -1), __ret);
	ASSERT(GFMRV_OK == gfmTilemap_loadStatic(hudBG, hudData, HUD_WIDTH_TILES, HUD_HEIGTH_TILES), __ret);

	rv = 0;
__ret:
	return rv;
}


void hud_free() {
	if (hudBG) {
		gfmTilemap_free(&hudBG);
	}
}


void hud_disable() {
	enabled = 0;
}


void hud_enable() {
	enabled = 1;
}


void hud_setActiveStones(enum rainbowColor colors) {
	active = colors;
}


void hud_setFuel(int max, int current, int recharging) {
	maxFuel = max;
	curFuel = current;
	slowBar = recharging;
}


/**
 * hud_drawFuelBar renders the fuel bar.
 *
 * @param [in]totalBars: The total number of bars that may be drawn.
 * @return 0: Success; Anything else: failure.
 */
static int hud_drawFuelBar(int totalBars) {
	int rv = 1;
	int x = HUD_BARS_X;
	int y = HUD_BARS_Y;
	int i, diff, tmp, tile;

	diff = maxFuel / totalBars;
	tmp = curFuel;

	if (slowBar) {
		tile = HUD_BARS_RELOAD_FIRST_TILE;
	}
	else {
		tile = HUD_BARS_FIRST_TILE;
	}

	for (i = 0; i < totalBars && diff <= tmp; i++) {
		ASSERT(
			GFMRV_OK == gfm_drawTile(
				gameCtx
				, gfx8x8
				, x
				, y
				, tile + i / HUD_BAR_PER_COLOR
				, 0 /*isFlipped*/
			)
			, __ret
		);

		x += HUD_BARS_OFFSET;
		tmp -= diff;
	}

	ASSERT(
		GFMRV_OK == gfm_drawTile(
			gameCtx
			, gfx8x8
			, HUD_BARS_OVERLAY_X
			, HUD_BARS_Y
			, HUD_BARS_OVERLAY_TILE
			, 0 /*isFlipped*/
		)
		, __ret
	);

	rv = 0;
__ret:
	return rv;
}


int hud_draw(struct scene *scene) {
	int rv = 1;

	if (enabled) {
		int cx = 0;
		int cy = 0;
		int numColors = 0;
		int i, tmpCollected, tmpActive;

		ASSERT_OK(camera_worldToScreen(&cx, &cy), __ret);
		cx *= -1;
		cy *= -1;

		ASSERT(GFMRV_OK == gfmTilemap_setPosition(hudBG, cx, cy), __ret);
		ASSERT(GFMRV_OK == gfmTilemap_draw(hudBG, gameCtx), __ret);

		/* Draw the collected/active stones. */
		tmpCollected = global_getPermanent().stones;
		tmpActive = active;
		for (i = 0; tmpCollected != 0 || tmpActive != 0; i++) {
			struct stone stone = stonesLUT[i];
			int tile = -1;

			if ((stone.color & tmpActive) != 0) {
				tile = stone.activeTile;
			}
			else if ((stone.color & tmpCollected) != 0) {
				tile = stone.collectedTile;
			}

			tmpActive &= ~stone.color;
			tmpCollected &= ~stone.color;
			if (tile == -1) {
				continue;
			}
			numColors++;

			ASSERT(
				GFMRV_OK == gfm_drawTile(
					gameCtx
					, gfx8x8
					, stone.x
					, stone.y
					, tile
					, 0 /*isFlipped*/
				)
				, __ret
			);
		}

		if (curFuel > 0) {
			ASSERT_OK(hud_drawFuelBar(numColors * HUD_BAR_PER_COLOR), __ret);
		}
	}

	rv = 0;
__ret:
	return rv;
}


struct stone stonesLUT[] = {
	{color: RED_COLOR   , x:  1, y:  5, activeTile: 4012, collectedTile: 4076},
	{color: ORANGE_COLOR, x:  9, y:  1, activeTile: 4013, collectedTile: 4077},
	{color: YELLOW_COLOR, x: 17, y:  5, activeTile: 4014, collectedTile: 4078},
	{color: GREEN_COLOR , x: 17, y: 15, activeTile: 4015, collectedTile: 4079},
	{color: CYAN_COLOR  , x:  9, y: 10, activeTile: 4016, collectedTile: 4080},
	{color: BLUE_COLOR  , x:  9, y: 19, activeTile: 4017, collectedTile: 4081},
	{color: PURPLE_COLOR, x:  1, y: 15, activeTile: 4018, collectedTile: 4082},
};
