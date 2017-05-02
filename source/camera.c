#include "camera.h"

#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdlib.h>

// forward declare private function(s)

bool camera_keyboard_callback(int key, enum keyboard_action action, void* user);

//bool camera_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user);

bool camera_mouse_move_cb(const struct mouse* mouse, void* user);

struct camera* camera_create(struct window* window, struct keyboard* keyboard, struct mouse* mouse) // , struct hud* hud
{
	assert(window);
	assert(keyboard);
	assert(mouse);
	//assert(hud);

	struct camera* self = calloc(1, sizeof(*self));

	assert(self);

	self->up = GLKVector3Make(0.0f, 1.0f, 0.0f);
	self->direction = GLKVector3Make(0.0f, 0.0f, 1.0f);
	self->position = GLKVector3Make(0.0f, 0.0f, 0.0f);

	self->target = GLKVector3Add(self->position, self->direction);

	self->rotation_y = 0.0;
	self->rotation_x = 0.0;

	self->window = window;
	self->keyboard = keyboard;
	self->mouse = mouse;
	//self->hud = hud;

	keyboard_push_callback(keyboard, camera_keyboard_callback, self);
	mouse_push_move_callback(mouse, camera_mouse_move_cb, self);
	//mouse_push_callback(mouse, camera_mouse_callback, self);

	return self;
}

GLKMatrix4 camera_look_through(struct camera* self)
{
	return GLKMatrix4MakeLookAt(
		self->position.x,
		self->position.y,
		self->position.z,

		self->target.x,
		self->target.y,
		self->target.z,

		self->up.x,
		self->up.y,
		self->up.z
		);
}

void camera_set_position(struct camera* self, GLKVector3 position)
{
	assert(self);

	self->position = position;

	self->target = GLKVector3Add(self->position, self->direction);
}

bool camera_keyboard_callback(int key, enum keyboard_action action, void* user)
{
	struct camera* self = user;

	/*
	if (action == KEYBOARD_RELEASE && key == GLFW_KEY_ESCAPE)
	{
		hud_register_callbacks(self->hud);
		return;
	}
	*/

	if (action == KEYBOARD_PRESS || action == KEYBOARD_HOLD)
	{
		switch (key)
		{
			case 'W':
			{
				GLKVector3 distance = GLKVector3MultiplyScalar(self->direction, CAMERA_MOVEMENT_SPEED);
				self->position = GLKVector3Add(self->position, distance);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
			case 'A':
			{
				GLKVector3 delta = GLKVector3CrossProduct(self->up, self->direction);
				delta = GLKVector3Normalize(delta);
				delta = GLKVector3MultiplyScalar(delta, CAMERA_STRAFE_SPEED);

				self->position = GLKVector3Add(self->position, delta);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
			case 'S':
			{
				GLKVector3 distance = GLKVector3MultiplyScalar(self->direction, -CAMERA_MOVEMENT_SPEED);
				self->position = GLKVector3Add(self->position, distance);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
			case 'D':
			{
				GLKVector3 delta = GLKVector3CrossProduct(self->direction, self->up);
				delta = GLKVector3Normalize(delta);
				delta = GLKVector3MultiplyScalar(delta, CAMERA_STRAFE_SPEED);

				self->position = GLKVector3Add(self->position, delta);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
			case ' ':
			{
				GLKVector3 distance = GLKVector3MultiplyScalar(self->up, CAMERA_RISE_SPEED);
				self->position = GLKVector3Add(self->position, distance);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
			case GLFW_KEY_LEFT_SHIFT:
			{
				GLKVector3 distance = GLKVector3MultiplyScalar(self->up, -CAMERA_SINK_SPEED);
				self->position = GLKVector3Add(self->position, distance);
				self->target = GLKVector3Add(self->position, self->direction);

				return true;
			}
		}
	}

	return false;
}

bool camera_mouse_move_cb(const struct mouse* mouse, void* user)
{
	assert(user);

	struct camera* self = user;

	self->rotation_y -= (mouse->x - self->window->width_half) / CAMERA_HORIZONTAL_SENSITIVITY;
	self->rotation_x -= (mouse->y - self->window->height_half) / CAMERA_VERTICAL_SENSITIVITY;

	window_center_cursor(self->window);

	if (self->rotation_x > 1.0f)
	{
		self->rotation_x = 1.0f;
	}
	else if (self->rotation_x < -1.0f)
	{
		self->rotation_x = -1.0f;
	}

	self->direction.x = sin(self->rotation_y);
	self->direction.z = cos(self->rotation_y);
	self->direction.y = sin(self->rotation_x);

	self->direction = GLKVector3Normalize(self->direction);

	self->target = GLKVector3Add(self->position, self->direction);

	self->up = GLKVector3CrossProduct(GLKVector3CrossProduct(self->direction, GLKVector3Make(0.0f, 1.0f, 0.0f)), self->direction);

	return true;
}

/*
bool camera_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user)
{
	if (action == MOUSE_MOVE)
	{
		struct camera* self = user;

		self->rotation_y -= (mouse->x - self->window->width_half) / CAMERA_HORIZONTAL_SENSITIVITY;
		self->rotation_x -= (mouse->y - self->window->height_half) / CAMERA_VERTICAL_SENSITIVITY;

		window_center_cursor(self->window);

		if (self->rotation_x > 1.0f)
		{
			self->rotation_x = 1.0f;
		}
		else if (self->rotation_x < -1.0f)
		{
			self->rotation_x = -1.0f;
		}

		self->direction.x = sin(self->rotation_y);
		self->direction.z = cos(self->rotation_y);
		self->direction.y = sin(self->rotation_x);

		self->direction = GLKVector3Normalize(self->direction);

		self->target = GLKVector3Add(self->position, self->direction);

		self->up = GLKVector3CrossProduct(GLKVector3CrossProduct(self->direction, GLKVector3Make(0.0f, 1.0f, 0.0f)), self->direction);

		return true;
	}

	return false;
}
//*/
