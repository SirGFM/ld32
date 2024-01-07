#ifndef SETUP_H
#define SETUP_H

/**
 * setup_init prepares the game's core,
 * loading all configurations, assets, etc.
 *
 * @return 0: Success; Anything else: failure.
 */
int setup_init();

/**
 * setup_free releases every resource associated with the game
 * (that was loaded during the setup).
 */
void setup_free();

#endif /* SETUP_H */
