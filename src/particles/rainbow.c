#include <core/assets.h>
#include <core/core.h>
#include <core/types.h>
#include <collision.h>
#include <error.h>
#include <particles/rainbow.h>
#include <scene.h>
#include <util.h>

#include <GFraMe/gfmGroup.h>
#include <GFraMe/gfmSprite.h>

#include <stdint.h>
#include <math.h>


/** Absolute speed for each rainbow particle. */
#define RAINBOW_SPEED 150

/** For how long each rainbow particle should live. */
#define RAINBOW_TTL_MS (1000 / rainbowAnim[1] * (rainbowAnim[0] + 1))

/** For how a explosing rainbow particle should live. */
#define RAINBOW_EXPLOSION_TTL_MS 400

/** The first animation frame with an exploding particle. */
#define RAINBOW_EXPLODE_FRAME 4

/** Angle of the arc made by the particles, in degrees. */
#define RAINBOW_ANGLE 24.0

/**
 * Maximum number of rainbow particles on screen at once.
 * TODO: Make this configurable.
 */
#define MAX_RAINBOW 4000


enum rainbowAnimation {
	  ANIM_RED_BULLET = 0
	, ANIM_ORANGE_BULLET
	, ANIM_YELLOW_BULLET
	, ANIM_GREEN_BULLET
	, ANIM_CYAN_BULLET
	, ANIM_BLUE_BULLET
	, ANIM_PURPLE_BULLET
	, ANIM_RED_EXPLOSION
	, ANIM_ORANGE_EXPLOSION
	, ANIM_YELLOW_EXPLOSION
	, ANIM_GREEN_EXPLOSION
	, ANIM_CYAN_EXPLOSION
	, ANIM_BLUE_EXPLOSION
	, ANIM_PURPLE_EXPLOSION
	, ANIM_MAX_PART_ANIM
};


/** Rainbow particles, mainly used by the player. */
static gfmGroup *rainbow = 0;


/** Tracks when the particle was last spawned. */
static int lastSpawnedMs = 0;


/** Animation data for each rainbow particle. */
static int rainbowAnim[];


/** Number entries in the rainbow animation data. */
static const int rainbowAnimLen;


/** Data for each animation. */
static int *rainbowAnimationData[ANIM_MAX_PART_ANIM] = {};


int rainbow_init() {
	static gfmGroup *tmpRainbow = 0;
	int rv = 1;
	int i;

	/* Alloc the particles and set its default attributes. */
	ASSERT(GFMRV_OK == gfmGroup_getNew(&tmpRainbow), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDefSpriteset(tmpRainbow, gfx8x8), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDefDimensions(tmpRainbow, 4, 4, -2, -2), __ret);
	ASSERT(
		GFMRV_OK == gfmGroup_setDefAnimData(
			tmpRainbow
			, rainbowAnim
			, rainbowAnimLen
		)
		, __ret
	);
	ASSERT(GFMRV_OK ==  gfmGroup_setDeathOnLeave(tmpRainbow, 0), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setDeathOnTime(tmpRainbow, RAINBOW_TTL_MS), __ret);
	ASSERT(
		GFMRV_OK == gfmGroup_setCollisionQuality(
			tmpRainbow
			, gfmCollisionQuality_collideEverything
		)
		, __ret
	);
	ASSERT(GFMRV_OK == gfmGroup_setDefType(tmpRainbow, BTYP_PARTICLE), __ret);

	/* Pre-cache some sprites.
	 * TODO: Make this configurable.
	 */
	ASSERT(
		GFMRV_OK == gfmGroup_preCache(
			tmpRainbow
			, MAX_RAINBOW
			, MAX_RAINBOW
		)
		, __ret
	);
	ASSERT(
		GFMRV_OK == gfmGroup_setDrawOrder(tmpRainbow, gfmDrawOrder_newestFirst)
		, __ret
	);

	rainbow = tmpRainbow;
	tmpRainbow = 0;

	/* Initialize the animation data helper. */
	rainbowAnimationData[0] = rainbowAnim;
	for (i = 1; i < ANIM_MAX_PART_ANIM; i++) {
		int len = rainbowAnimationData[i - 1][0];
		rainbowAnimationData[i] = rainbowAnimationData[i - 1] + len + 3;
	}

	rv = 0;
__ret:
	if (tmpRainbow) {
		gfmGroup_free(&tmpRainbow);
	}

	return rv;
}


void rainbow_free() {
	if (rainbow) {
		gfmGroup_free(&rainbow);
		rainbow = 0;
	}
}


int rainbow_reset() {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_killAll(rainbow), __ret);

	rv = 0;
__ret:
	return rv;
}


int rainbow_update(struct scene *scene) {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_update(rainbow, gameCtx), __ret);
	ASSERT_OK(collision_collideGroup(scene->staticCollider, rainbow), __ret);
	ASSERT_OK(collision_collideGroup(scene->dynamicCollider, rainbow), __ret);

	if (lastSpawnedMs > 0) {
		lastSpawnedMs -= scene->elapsedMs;
	}

	rv = 0;
__ret:
	return rv;
}


int rainbow_draw(struct scene *scene) {
	int rv = 1;

	ASSERT(GFMRV_OK == gfmGroup_draw(rainbow, gameCtx), __ret);

	rv = 0;
__ret:
	return rv;
}


/**
 * rainbow_spawnBullet spawns a single rainbow bullet of the provided color.
 *
 * @param [in] color: The color of the particle.
 * @param [in] angle: The angle at which the particle is being shot.
 * @param [in] cx: The horizontal position of the particle's center.
 * @param [in] cy: The veritcal position of the particle's center.
 * @return 0: Success; Anything else: failure.
 */
static int rainbow_spawnBullet(enum rainbowColor color, double angle, int cx, int cy) {
	gfmSprite *tmp;
	double da, va, vx, vy;
	enum type type;
	int anim;
	int rv = 1;

	/* Convert the color to an animation. */
	switch (color) {
	#define SET_ANIM(color) \
		case color ## _COLOR: \
			anim = ANIM_ ## color ## _BULLET; \
			type = TYP_PARTICLE_ ## color; \
			break

	SET_ANIM(RED);
	SET_ANIM(ORANGE);
	SET_ANIM(YELLOW);
	SET_ANIM(GREEN);
	SET_ANIM(CYAN);
	SET_ANIM(BLUE);
	SET_ANIM(PURPLE);

	#undef SET_ANIM
	default:
		ASSERT(0 /* Invalid color */, __ret);
	}

	/* Slightly move the angle around. */
	da = (util_prng() % 100 - 50) / 250.0;
	da = da * PI / 180.0;

	/* Also slight change the velocity by at most 5%. */
	va = 1.0 + (util_prng() % 100 - 50) / 1000.0;

	vx = va * RAINBOW_SPEED * cos(angle + da);
	vy = va * RAINBOW_SPEED * sin(angle + da);

	/* TODO: Handle empty group? */
	ASSERT(GFMRV_OK == gfmGroup_recycle(&tmp, rainbow), __ret);

	ASSERT(GFMRV_OK == gfmGroup_setType(rainbow, type), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setPosition(rainbow, cx, cy), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setAnimation(rainbow, anim), __ret);
	ASSERT(GFMRV_OK == gfmGroup_setVelocity(rainbow, vx, vy), __ret);

	rv = 0;
__ret:
	return rv;
}


/**
 * rainbow_prepareSpawn calculates the parameters required to shot rainbow particles.
 *
 * It calculates:
 *
 *   - The initial angle
 *   - The angle difference between particles shot at once
 *   - The particles initial position
 *
 * @param [out] angle: The particle's initial angle.
 * @param [out] delta: The angle difference between particles.
 * @param [in/out] cx: The horizontal position of the spawner's center.
 * @param [in/out] cy: The vertical position of the spawner's center.
 * @param [in] dx: The cosine of the particle's shooting direction.
 * @param [in] dy: The sine of the particle's shooting direction.
 * @param [in] dist: Distance from the spawner's center, at which the particles are spawned.
 * @param [in] num: The number of particles that will be spawned.
 */
static void rainbow_prepareSpawn(
	double *angle
	, double *delta
	, int *cx
	, int *cy
	, double dx
	, double dy
	, int dist
	, int num
) {
	/* Calculate the angle difference between particles, in radians. */
	if (num > 1) {
		double rad = RAINBOW_ANGLE * PI / 180.0;
		*delta = rad / (double)num;
	}
	else {
		*delta = 0.0;
	}

	/* Calculate the shooting position
	 * (in a circle around the center point). */
	*cx += (int)(dx * dist);
	*cy += (int)(dy * dist);

	/* Calculate the initial shooting angle, in radians,
	 * ensuring that 90 degrees is up. */
	*angle = atan2(dx, -dy);
	*angle -= *delta * num / 2.0;
	*angle -= PI / 2.0;
}


int rainbow_spawn(enum rainbowColor colors, double dx, double dy, int cx, int cy, int dist) {
	double angle, delta;
	int num;
	enum rainbowColor curColor;
	int rv = 1;

	num = countBits((uint32_t)colors);
	if (num == 0 || lastSpawnedMs > 0) {
		rv = 0;
		goto __ret;
	}

	rainbow_prepareSpawn(&angle, &delta, &cx, &cy, dx, dy, dist, num);

	/* Iterate over every possible color,
	 * spawning particles when the current color matches a provided color. */
	curColor = 1;
	while (colors != 0) {
		if (colors & 1) {
			ASSERT_OK(rainbow_spawnBullet(curColor, angle, cx, cy), __ret);
			angle += delta;
		}

		colors >>= 1;
		curColor <<= 1;
	}

	rv = 0;
__ret:
	return rv;
}


int rainbow_spawnSingleColor(
	enum rainbowColor color
	, int count
	, double dx
	, double dy
	, int cx
	, int cy
	, int dist
) {
	double angle, delta;
	int rv = 1;

	if (count == 0 || lastSpawnedMs > 0) {
		rv = 0;
		goto __ret;
	}

	rainbow_prepareSpawn(&angle, &delta, &cx, &cy, dx, dy, dist, count);

	/* Iterate over every possible color,
	 * spawning particles when the current color matches a provided color. */
	while (count > 0) {
		ASSERT_OK(rainbow_spawnBullet(color, angle, cx, cy), __ret);
		angle += delta;
		count--;
	}

	rv = 0;
__ret:
	return rv;
}


int rainbow_explodeBullet(gfmSprite *bullet) {
	void *ptr;
	gfmGroupNode *node;
	int rv = 1;
	int frame, index, tmp;

	ASSERT(GFMRV_OK == gfmSprite_getAnimationIndex(&index, bullet), __ret);

	/* Check if the animation has already been updated. */
	if (index >= ANIM_RED_EXPLOSION) {
		return 0;
	}

	/* Update the animation, unless it's already exploding. */
	ASSERT(GFMRV_OK == gfmSprite_getFrame(&frame, bullet), __ret);
	frame = frame - rainbowAnimationData[index][3];

	if (frame < RAINBOW_EXPLODE_FRAME) {
		index += ANIM_RED_EXPLOSION - ANIM_RED_BULLET;
		ASSERT(GFMRV_OK == gfmSprite_playAnimation(bullet, index), __ret);

		/* Lower the particle's TTL to match the animation. */
		ASSERT(GFMRV_OK == gfmSprite_getChild(&ptr, &tmp, bullet), __ret);
		node = (gfmGroupNode*)ptr;

		ASSERT(GFMRV_OK == gfmGroup_setTimeToLive(node, RAINBOW_EXPLOSION_TTL_MS), __ret);
	}

	rv = 0;
__ret:
	return rv;
}


/* === Mapping of a rainbow's tile to its animation frame =================== */
#define RED_1  266
#define RED_2  267
#define RED_3  268
#define RED_4  269
#define RED_5  270
#define RED_6  330
#define RED_7  331
#define RED_8  332
#define RED_9  333
#define RED_10 334
#define ORA_1  394
#define ORA_2  395
#define ORA_3  396
#define ORA_4  397
#define ORA_5  398
#define ORA_6  458
#define ORA_7  459
#define ORA_8  460
#define ORA_9  461
#define ORA_10 462
#define YEL_1  522
#define YEL_2  523
#define YEL_3  524
#define YEL_4  525
#define YEL_5  526
#define YEL_6  586
#define YEL_7  587
#define YEL_8  588
#define YEL_9  589
#define YEL_10 590
#define GRE_1  650
#define GRE_2  651
#define GRE_3  652
#define GRE_4  653
#define GRE_5  654
#define GRE_6  714
#define GRE_7  715
#define GRE_8  716
#define GRE_9  717
#define GRE_10 718
#define CYA_1  778
#define CYA_2  779
#define CYA_3  780
#define CYA_4  781
#define CYA_5  782
#define CYA_6  842
#define CYA_7  843
#define CYA_8  844
#define CYA_9  845
#define CYA_10 846
#define BLU_1  906
#define BLU_2  907
#define BLU_3  908
#define BLU_4  909
#define BLU_5  910
#define BLU_6  970
#define BLU_7  971
#define BLU_8  972
#define BLU_9  973
#define BLU_10 974
#define PUR_1  1034
#define PUR_2  1035
#define PUR_3  1036
#define PUR_4  1037
#define PUR_5  1038
#define PUR_6  1098
#define PUR_7  1099
#define PUR_8  1100
#define PUR_9  1101
#define PUR_10 1102


/** Animation data for each rainbow particle. */
static int rainbowAnim[] = {
/*                   */ /*len|fps|loop|frames...*/
/*     RED_BULLET    */    24, 15,  0 , RED_1,RED_2,RED_3,RED_4, RED_3,RED_4,RED_3,RED_4,RED_3,RED_4, RED_4,RED_4,RED_4,RED_4,RED_4,RED_4, RED_3,RED_4,RED_5,RED_6, RED_7,RED_8,RED_9,RED_10,
/*   ORANGE_BULLET   */    24, 15,  0 , ORA_1,ORA_2,ORA_3,ORA_4, ORA_3,ORA_4,ORA_3,ORA_4,ORA_3,ORA_4, ORA_4,ORA_4,ORA_4,ORA_4,ORA_4,ORA_4, ORA_3,ORA_4,ORA_5,ORA_6, ORA_7,ORA_8,ORA_9,ORA_10,
/*   YELLOW_BULLET   */    24, 15,  0 , YEL_1,YEL_2,YEL_3,YEL_4, YEL_3,YEL_4,YEL_3,YEL_4,YEL_3,YEL_4, YEL_4,YEL_4,YEL_4,YEL_4,YEL_4,YEL_4, YEL_3,YEL_4,YEL_5,YEL_6, YEL_7,YEL_8,YEL_9,YEL_10,
/*    GREEN_BULLET   */    24, 15,  0 , GRE_1,GRE_2,GRE_3,GRE_4, GRE_3,GRE_4,GRE_3,GRE_4,GRE_3,GRE_4, GRE_4,GRE_4,GRE_4,GRE_4,GRE_4,GRE_4, GRE_3,GRE_4,GRE_5,GRE_6, GRE_7,GRE_8,GRE_9,GRE_10,
/*    CYAN_BULLET    */    24, 15,  0 , CYA_1,CYA_2,CYA_3,CYA_4, CYA_3,CYA_4,CYA_3,CYA_4,CYA_3,CYA_4, CYA_4,CYA_4,CYA_4,CYA_4,CYA_4,CYA_4, CYA_3,CYA_4,CYA_5,CYA_6, CYA_7,CYA_8,CYA_9,CYA_10,
/*    BLUE_BULLET    */    24, 15,  0 , BLU_1,BLU_2,BLU_3,BLU_4, BLU_3,BLU_4,BLU_3,BLU_4,BLU_3,BLU_4, BLU_4,BLU_4,BLU_4,BLU_4,BLU_4,BLU_4, BLU_3,BLU_4,BLU_5,BLU_6, BLU_7,BLU_8,BLU_9,BLU_10,
/*   PURPLE_BULLET   */    24, 15,  0 , PUR_1,PUR_2,PUR_3,PUR_4, PUR_3,PUR_4,PUR_3,PUR_4,PUR_3,PUR_4, PUR_4,PUR_4,PUR_4,PUR_4,PUR_4,PUR_4, PUR_3,PUR_4,PUR_5,PUR_6, PUR_7,PUR_8,PUR_9,PUR_10,
/*   RED_EXPLOSION   */    6 , 15,  0 , RED_5,RED_6,RED_7,RED_8, RED_9,RED_10,
/* ORANGE_EXPLOSION  */    6 , 15,  0 , ORA_5,ORA_6,ORA_7,ORA_8, ORA_9,ORA_10,
/* YELLOW_EXPLOSION  */    6 , 15,  0 , YEL_5,YEL_6,YEL_7,YEL_8, YEL_9,YEL_10,
/*  GREEN_EXPLOSION  */    6 , 15,  0 , GRE_5,GRE_6,GRE_7,GRE_8, GRE_9,GRE_10,
/*  CYAN_EXPLOSION   */    6 , 15,  0 , CYA_5,CYA_6,CYA_7,CYA_8, CYA_9,CYA_10,
/*  BLUE_EXPLOSION   */    6 , 15,  0 , BLU_5,BLU_6,BLU_7,BLU_8, BLU_9,BLU_10,
/* PURPLE_EXPLOSION  */    6 , 15,  0 , PUR_5,PUR_6,PUR_7,PUR_8, PUR_9,PUR_10,
/*   MAX_PART_ANIM   */    1 ,  0,  0 , -1,
};


/** Number entries in the rainbow animation data. */
static const int rainbowAnimLen = sizeof(rainbowAnim) / sizeof(int);
