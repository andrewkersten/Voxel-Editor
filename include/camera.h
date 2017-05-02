#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "GLKMath.h"

//#include "hud.h"
#include "keyboard.h"
#include "mouse.h"
#include "window.h"

#define CAMERA_MOVEMENT_SPEED (20.0f / 100.0f)
#define CAMERA_STRAFE_SPEED CAMERA_MOVEMENT_SPEED
#define CAMERA_RISE_SPEED CAMERA_MOVEMENT_SPEED
#define CAMERA_SINK_SPEED CAMERA_RISE_SPEED
#define CAMERA_HORIZONTAL_SENSITIVITY 800.0f
#define CAMERA_VERTICAL_SENSITIVITY 1200.0f

struct camera
{
	GLKVector3 position;
	GLKVector3 target;
	GLKVector3 up;

	GLKVector3 direction;

	double rotation_y;
	double rotation_x;

	struct window* window;
	struct keyboard* keyboard;
	struct mouse* mouse;
	//struct hud* hud;
};

/*!
* @brief Allocate memory for and initialize a camera.
*
* The camera is placed at the origin looking down the positive z axis, with
* the up vector pointing in the positive y axis, and no rotation.  The camera
* registers mouse and keyboard event callbacks here.
*
* @param window The window the camera registers callbacks for input events with.
* @return A pointer to the camera structure or @c NULL on failure.
*/
struct camera* camera_create(struct window* window, struct keyboard* keyboard, struct mouse* mouse); //, struct hud* hud

/*!
* @brief Calculate the view matrix from the current camera state.
*
* @param camera The camera structure.
* @return The view matrix calculated from the current camera state.
*/
GLKMatrix4 camera_look_through(struct camera* camera);

/*!
* @brief Set the camera position.
*
* @param camera The camera structure.
* @param position The new position.
*/
void camera_set_position(struct camera* camera, GLKVector3 position);

#endif
