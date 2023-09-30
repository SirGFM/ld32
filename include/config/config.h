#ifndef CONFIG_H
#define CONFIG_H

#include <GFraMe/gframe.h>


/** The base virtual width of the game. */
#define VWIDTH 320
/** The base virtual height of the game. */
#define VHEIGHT 240
/** The default update rate, in frames per second. */
#define DEFAULT_FPS 60
/** The default draw rate, in frames per second. */
#define DEFAULT_DPS 60
/** Name of the configuration file. */
#define CONFIG_FILE "config"


/** The game's configuration. */
struct config {
	/** Which video backend should be used. */
	gfmVideoBackend videoBackend;
	/** Which audio mode should be used. */
	gfmAudioQuality audioMode;
	/** Whether VSync should be enable. */
	int vsync;
	/** The initial window width. */
	int winWidth;
	/** The initial window height. */
	int winHeight;
	/** How often should the game logic be updated. */
	int updateRate;
	/** How often should the game be drawn. */
	int drawRate;
	/** Whether this configuration was loaded from a file. */
	int wasLoaded;
};


/**
 * config_load checks if there's any saved configuration file and loads it.
 *
 * If no configuration is found, the default configuration is returned!
 *
 * If core hasn't been initialized yet,
 * then it's temporarily initialized by the function.
 *
 * @param [out] cfg: The loaded configuration.
 * @return 0: Success; Anything else: failure.
 */
int config_load(struct config *cfg);


#endif /* CONFIG_H */
