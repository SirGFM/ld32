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
    /** Camera's width */
    int viewWidth;
    /** Camera's height */
    int viewHeight;
    /** World's width */
    int worldWidth;
    /** World's height */
    int worldHeight;
    /** Dead zone's center position */
    int deadX;
    /** Dead zone's center position */
    int deadY;
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
    
    pCam->deadX = pCam->viewWidth / 2;
    pCam->deadY = pCam->viewHeight / 2;
    pCam->deadWidth = width / 2;
    pCam->deadHeight = height / 2;
__ret:
    return;
}

/**
 * Center the camera at a position
 * 
 * @return Returns 1 if it just pushed into the deadzone
 */
int cam_centerAt(camera *pCam, int x, int y) {
    int rv, dx, dy;
    
    rv = 0;
    
    // Check arguments
    ASSERT(pCam, 0);
    
    // Check the distance to the camera
    dx = x - pCam->x - pCam->deadX;
    dy = y - pCam->y - pCam->deadY;
    
    // 'Center' it at the position
    if (dy > pCam->deadHeight || dy < -pCam->deadHeight) {
        pCam->y = y - pCam->deadY;
        if (dy > 0)
            pCam->y -= pCam->deadHeight;
        else if (dy < 0)
            pCam->y += pCam->deadHeight;
        rv = 1;
    }
    
    // Check that it's in-bounds
    if (pCam->y < 0) {
        pCam->y = 0;
        rv = 0;
    }
    else if (pCam->y + pCam->viewHeight > pCam->worldHeight) {
        pCam->y = pCam->worldHeight - pCam->viewHeight;
        rv = 0;
    }
    
    if (dx > pCam->deadWidth || dx < -pCam->deadWidth) {
        pCam->x = x - pCam->deadX;
        if (dx > 0)
            pCam->x -= pCam->deadWidth;
        else if (dx < 0)
            pCam->x += pCam->deadWidth;
        rv = 1;
    }
    if (pCam->x < 0) {
        pCam->x = 0;
        rv = 0;
    }
    else if (pCam->x + pCam->viewWidth > pCam->worldWidth) {
        pCam->x = pCam->worldWidth - pCam->viewWidth;
        rv = 0;
    }
    
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

