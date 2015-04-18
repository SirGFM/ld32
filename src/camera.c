/**
 * @file src/camera.c
 * 
 * Camera that defines which are to render
 */
#include "camera.h"

#include <stdlib.h>
#include <string.h>

#include "global.h"

/** 'Export' the camera structure */
struct stCamera {
    /** Camera's position into the world */
    int x;
    /** Camera's position into the world */
    int y;
    /** Last position the camera was set to */
    int lastCenterX;
    /** Last position the camera was set to */
    int lastCenterY;
    /** Camera's width */
    int viewWidth;
    /** Camera's height */
    int viewHeight;
    /** World's width */
    int worldWidth;
    /** World's height */
    int worldHeight;
    /** Dead zone's width */
    int deadWidth;
    /** Dead zone's height */
    int deadHeight;
};

/**
 * Alloc a new camera 'object'
 */
int cam_getNew(camera **ppCam) {
    int rv;
    
    // Check the arguments
    ASSERT(ppCam, 1);
    ASSERT(!(*ppCam), 1);
    
    // Alloc the camera
    *ppCam = (camera*)malloc(sizeof(camera));
    ASSERT(*ppCam, 1);
    
    // Initialize everything to 0
    memset(*ppCam, 0, sizeof(camera));
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Free a camera 'object'
 */
void cam_free(camera **ppCam) {
    // Check the arguments
    ASSERT_NR(ppCam);
    ASSERT_NR(*ppCam);
    
    // Free the camera
    free(*ppCam);
    *ppCam = 0;
__ret:
    return;
}

/**
 * Initialize the camera, defining its limits and dimensions
 */
void cam_init(camera *pCam, int viewWidth, int viewHeight, int worldWidth, int worldHeight) {
    // Check the arguments
    ASSERT_NR(pCam);
    
    pCam->viewWidth = viewWidth;
    pCam->viewHeight = viewHeight;
    pCam->worldWidth = worldWidth;
    pCam->worldHeight = worldHeight;
__ret:
    return;
}

/**
 * Get the camera's params
 */
void cam_getParams(int *pX, int *pY, int *pW, int *pH, camera *pCam) {
    // Check the arguments
    ASSERT_NR(pCam);
    ASSERT_NR(pX);
    ASSERT_NR(pY);
    ASSERT_NR(pW);
    ASSERT_NR(pH);
    
    cam_getPos(pX, pY, pCam);
    cam_getDimensions(pW, pH, pCam);
__ret:
    return;
}

/**
 * Get the camera's position
 */
void cam_getPos(int *pX, int *pY, camera *pCam) {
    // Check the arguments
    ASSERT_NR(pCam);
    ASSERT_NR(pX);
    ASSERT_NR(pY);
    
    *pX = pCam->x;
    *pY = pCam->y;
__ret:
    return;
}

/**
 * Get the camera's dimentions
 */
void cam_getDimensions(int *pW, int *pH, camera *pCam) {
    // Check the arguments
    ASSERT_NR(pCam);
    ASSERT_NR(pW);
    ASSERT_NR(pH);
    
    *pW = pCam->viewWidth;
    *pH = pCam->viewHeight;
__ret:
    return;
}

/**
 * Set the position at which the camera should move
 */
void cam_setDeadzone(camera *pCam, int width, int height) {
    // Check the arguments
    ASSERT_NR(pCam);
    
    pCam->deadWidth = width;
    pCam->deadHeight = height;
__ret:
    return;
}

/**
 * Center the camera at a position
 * 
 * @return Returns 1 if it just pushed into the deadzone
 */
int cam_centerAt(camera *pCam, int x, int y) {
    int rv;
    
    // Check arguments
    ASSERT(pCam, 0);
    
    // TODO cam_centerAt
    
    // Check if last position was in the deadzone
    // if (true) center at (x,y) (i.e., "reset") and return 0
    // otherwise check if (x,y) is in the deadzone
    //   if (true) push (x,y) outside the deadzone (check the movement) and return 1
    //   otherwise, do nothing and return 0
    
    rv = 0;
__ret:
    return rv;
}

/**
 * Returns whether an object is inside a camera
 */
int cam_isInside(camera *pCam, GFraMe_object *pObj) {
    // TODO cam_isInside
    return 1;
}

