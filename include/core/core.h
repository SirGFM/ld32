#ifndef CORE_H
#define CORE_H

#include <GFraMe/gframe.h>
#include <config/config.h>


/** Name of the "organization" making the game. */
#define ORG "com.gfmgamecorner"
/** The game's title. */
#define TITLE "Umbra's Rainbow Quest"
/** The maximum FPS, used to configure the game's internal scheduler. */
#define MAX_FPS 60


/** The main game context. */
extern gfmCtx *gameCtx;


/**
 * Release all resources associated with the game's libs.
 *
 * @return 0: Success; Anything else: failure.
 */
int core_free();


/**
 * core_initWindowless initializes core without actually creating a window,
 * so files (such as the configuration) may be loaded.
 *
 * @param [in] cfg: The game's configuration.
 * @return 0: Success; Anything else: failure.
 */
int core_initWindowless(struct config *cfg);


/**
 * core_init fully initialize core,
 * creating the game's window and initializing every subsystem.
 *
 * @param [in] cfg: The game's configuration.
 * @return 0: Success; Anything else: failure.
 */
int core_init(struct config *cfg);


#endif /* CORE_H */
