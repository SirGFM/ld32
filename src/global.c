#include <global.h>


static struct permanentData curPermanent = {
	.stones = RED_COLOR | ORANGE_COLOR | YELLOW_COLOR | GREEN_COLOR | CYAN_COLOR | BLUE_COLOR | PURPLE_COLOR
};


struct permanentData global_getPermanent() {
	return curPermanent;
}


void global_setPermanent(struct permanentData data) {
	curPermanent = data;
}
