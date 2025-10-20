#include <global.h>


/** The currently loaded permanent data. */
static struct permanentData curPermanent = {};


struct permanentData global_getPermanent() {
	return curPermanent;
}


void global_setPermanent(struct permanentData data) {
	curPermanent = data;
}


void global_addRedStone() {
	curPermanent.stones |= RED_COLOR;
}


void global_addOrangeStone() {
	curPermanent.stones |= ORANGE_COLOR;
}


void global_addYellowStone() {
	curPermanent.stones |= YELLOW_COLOR;
}


void global_addGreenStone() {
	curPermanent.stones |= GREEN_COLOR;
}


void global_addCyanStone() {
	curPermanent.stones |= CYAN_COLOR;
}


void global_addBlueStone() {
	curPermanent.stones |= BLUE_COLOR;
}


void global_addPurpleStone() {
	curPermanent.stones |= PURPLE_COLOR;
}
