/**
 * @file src/camera.h
 * 
 * Camera that defines which are to render
 */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <GFraMe/GFraMe_object.h>

/** 'Export' the camera structure */
typedef struct stCamera camera;

/**
 * Alloc a new camera 'object'
 */
int cam_getNew(camera **ppCam);
/**
 * Free a camera 'object'
 */
void cam_free(camera **ppCam);

/**
 * Initialize the camera, defining its limits and dimensions
 */
void cam_init(camera *pCam, int viewWidth, int viewHeight, int worldWidth, int worldHeight);

/**
 * Get the camera's params
 */
void cam_getParams(int *pX, int *pY, int *pW, int *pH, camera *pCam);

/**
 * Get the camera's position
 */
void cam_getPos(int *pX, int *pY, camera *pCam);

/**
 * Get the camera's dimentions
 */
void cam_getDimensions(int *pW, int *pH, camera *pCam);

/**
 * Set the position at which the camera should move
 */
void cam_setDeadzone(camera *pCam, int width, int height);

/**
 * Center the camera at a position
 * 
 * @return Returns 1 if it just pushed into the deadzone
 */
int cam_centerAt(camera *pCam, int x, int y);

/**
 * Returns whether an object is inside a camera
 */
int cam_isInside(camera *pCam, GFraMe_object *pObj);

/**
 * Transform a in-screen position to world coordinates
 */
void cam_screenToWorld(int *x, int *y, camera *pCam);

#endif /* __CAMERA_H__ */

