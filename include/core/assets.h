#ifndef ASSETS_H
#define ASSETS_H

#include <GFraMe/gframe.h>


/**
 * The game's texture. It must be within an 'assets/' directory,
 * besides tha game's binary.
 */
#define TEXTURE_ATLAS "atlas.bmp"
/** Color that is interpreted as transparent. */
#define TEXTURE_CHROMA_KEY 0xff00ff


/** Spritesheet for 8x8 sprites. */
extern gfmSpriteset *gfx8x8;
/** Spritesheet for 16x16 sprites. */
extern gfmSpriteset *gfx16x16;


/**
 * assets_loadGfx loads the game's texture
 * and configures all the available spritesheets.
 *
 * These assets are managed internally by the library,
 * and thus they don't have to be manually released.
 *
 * @return 0: Success; Anything else: failure.
 */
int assets_loadGfx();


#endif /* ASSETS_H */
