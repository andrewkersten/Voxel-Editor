#ifndef __MOUSE_H__
#define __MOUSE_H__

//#include "darray.h"
#include "window.h"

#include <stdbool.h>

//#define MOUSE_BUTTON_COUNT 3
#define MOUSE_CALLBACK_CAPACITY 16

/*
enum mouse_action
{
	MOUSE_MOVE,
	MOUSE_BUTTON_PRESS,
	MOUSE_BUTTON_RELEASE,
};
//*/

struct mouse;
struct mouse_button_state;

typedef bool(*mouse_move_callback)(const struct mouse* mouse, void* user);

typedef bool(*mouse_drag_callback)(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

typedef bool(*mouse_press_callback)(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

typedef bool(*mouse_hold_callback)(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

typedef bool(*mouse_release_callback)(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

typedef bool(*mouse_scroll_callback)(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

//typedef bool(*mouse_callback)(enum mouse_action action, struct mouse* mouse, int button, void* user);

enum mouse_button_type
{
	MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_1,
	MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_2,
	MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_3,
};

enum mouse_scroll_direction
{
	MOUSE_SCROLL_UP,
	MOUSE_SCROLL_DOWN,
};

struct mouse_button_state
{
	enum mouse_button_type type;

	enum mouse_scroll_direction scroll;

	bool released;
	bool pressed;
	bool was_dragged;

	double pressed_x;
	double pressed_y;

	double released_x;
	double released_y;
};

struct mouse_callback_bundle
{
	union
	{
		mouse_move_callback move_callback;
		mouse_drag_callback drag_callback;
		mouse_press_callback press_callback;
		mouse_hold_callback hold_callback;
		mouse_release_callback release_callback;
		mouse_scroll_callback scroll_callback;
	};

	void* user;

	//mouse_callback callback;
	//bool visible;
};

struct mouse
{
	//struct darray* callbacks;

	struct window* window;

	struct mouse_button_state state_left;
	struct mouse_button_state state_right;
	struct mouse_button_state state_middle;

	struct mouse_callback_bundle move_callbacks[MOUSE_CALLBACK_CAPACITY];
	struct mouse_callback_bundle drag_callbacks[MOUSE_CALLBACK_CAPACITY];
	struct mouse_callback_bundle press_callbacks[MOUSE_CALLBACK_CAPACITY];
	struct mouse_callback_bundle hold_callbacks[MOUSE_CALLBACK_CAPACITY];
	struct mouse_callback_bundle release_callbacks[MOUSE_CALLBACK_CAPACITY];
	struct mouse_callback_bundle scoll_callbacks[MOUSE_CALLBACK_CAPACITY];

	size_t move_callback_count;
	size_t drag_callback_count;
	size_t press_callback_count;
	size_t hold_callback_count;
	size_t release_callback_count;
	size_t scroll_callback_count;

	/*
	size_t callback_count;
	struct mouse_callback_bundle callbacks[MOUSE_CALLBACK_CAPACITY];

	struct mouse_button_state button_states[MOUSE_BUTTON_COUNT];
	//*/

	double x;
	double y;

	/*
	double x_prev;
	double y_prev;
	//*/
};

/*!
* @brief Allocate memory for and initialize the structure.
*
* @return A pointer to the structure or @c NULL on failure.
*/
struct mouse* mouse_create(struct window* window);

/*!
* @brief Deallocate memory allocated by the structure and the structure itself.
*
* @param self The structure instance.
*/
void mouse_destroy(struct mouse* self);

/*!
* @brief Register a callback be notified when a mouse event occurs.
*
* @param self The structure instance.
* @param callback The callback to register.
* @param user A piece of user data passed into the callback.
* @see mouse_event_callback
*/
//void mouse_register_callback(struct mouse* self, mouse_callback callback, void* user);

//void mouse_push_callback(struct mouse* self, mouse_callback callback, void* user);

//void mouse_pop_callback(struct mouse* self);

void mouse_push_move_callback(struct mouse* self, mouse_move_callback callback, void* user);
void mouse_push_drag_callback(struct mouse* self, mouse_drag_callback callback, void* user);
void mouse_push_press_callback(struct mouse* self, mouse_press_callback callback, void* user);
void mouse_push_hold_callback(struct mouse* self, mouse_hold_callback callback, void* user);
void mouse_push_release_callback(struct mouse* self, mouse_release_callback callback, void* user);
void mouse_push_scroll_callback(struct mouse* self, mouse_scroll_callback callback, void* user);

void mouse_pop_move_callback(struct mouse* self);
void mouse_pop_drag_callback(struct mouse* self);
void mouse_pop_press_callback(struct mouse* self);
void mouse_pop_hold_callback(struct mouse* self);
void mouse_pop_release_callback(struct mouse* self);
void mouse_pop_scroll_callback(struct mouse* self);

/*!
* @brief Update the mouse state and notify all registered callbacks that an
* event has occurred.
*
* @param self The structure instance.
*/
void mouse_tick(struct mouse* self);

void mouse_set_cursor_visible(struct mouse* self, bool visible);

void mouse_center_cursor(struct mouse* self);

#endif
