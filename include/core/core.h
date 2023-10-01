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


/**
 * core_runGame handles executing the game's mainloop.
 *
 * Initially, mainloop_init() is called to configure the game's initial state.
 *
 * Then, mainloop_update() and mainloop_draw() are called as many times as needed.
 * When running natively, this won't return until the game has finished!
 *
 * Lastly, mainloop_free() is called to release the game's resources.
 * However, if running on the web, this last step is skipped!
 *
 * @return 0: Success; Anything else: failure.
 */
int core_runGame();


#endif /* CORE_H */
