#include <global.h>


/** The currently loaded permanent data. */
static struct permanentData curPermanent = {};


struct permanentData global_getPermanent() {
	return curPermanent;
}


void global_setPermanent(struct permanentData data) {
	curPermanent = data;
}
