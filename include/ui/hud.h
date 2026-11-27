#ifndef HUD_H
#define HUD_H


#include <core/types.h>
#include <scene.h>


/**
 * hud_init initialize the game HUD.
 *
 * @return 0: Success; Anything else: failure.
 */
int hud_init();


/**
 * hud_free releases all resources associated with the game HUD.
 */
void hud_free();


/**
 * hud_disable disables the HUD, hiding it.
 */
void hud_disable();


/**
 * hud_enable enables the HUD, showing it.
 */
void hud_enable();


/**
 * hud_setActiveStones sets which stones are currently active.
 *
 * @param [in]colors: Which stones have are currently active.
 */
void hud_setActiveStones(enum rainbowColor colors);


/**
 * hud_setFuel sets the current amount of fuel available.
 *
 * @param[in]max: The maximum amount of fuel;
 * @param[in]current: How many fuel is currently left.
 */
void hud_setFuel(int max, int current);


/**
 * hud_draw draws the HUD.
 *
 * @return 0: Success; Anything else: failure.
 */
int hud_draw(struct scene *scene);


#endif /* HUD_H */
