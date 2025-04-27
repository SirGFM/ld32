#include <global.h>


static struct permanentData curPermanent;


struct permanentData global_getPermanent() {
	return curPermanent;
}


void global_setPermanent(struct permanentData data) {
	curPermanent = data;
}
