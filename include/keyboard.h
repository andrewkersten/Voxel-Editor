#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

//#include "darray.h"
#include "window.h"

#include <stdbool.h>

/*! 
@brief Number of keys monitored.
*/
#define KEYBOARD_KEY_COUNT 350
#define KEYBOARD_CALLBACK_CAPACITY 16

/*!
@brief Collection of possible keyboard event actions.
*/
enum keyboard_action
{
	/*! Triggered when a key is first pressed. */
	KEYBOARD_PRESS,

	/*! Triggered when a key is released. */
	KEYBOARD_RELEASE,

	/*! Triggered when a key is down every tick after KEYBOARD_PRESS has been
	* triggered.
	*/
	KEYBOARD_HOLD,

	/*! Triggered every tenth tick a key is held down. */
	KEYBOARD_REPEAT,
};

/*!
@brief Callback type used for keyboard events.  Register callbacks with a
keyboard.

@param key Key that triggered the event.
@param action Action that occured.
@param user User data that was registered with the callback.
@see keyboard_register_callback
*/
typedef bool(*keyboard_callback)(int key, enum keyboard_action action, void* user);

/*!
@brief Bundle a callback with a piece of user data.
*/
struct keyboard_callback_bundle
{
	keyboard_callback callback;
	void* user;
};

/*!
@brief Stores the current state of a single key.
*/
struct keyboard_key_state
{
	/*! @c true if the key was just released. */
	bool released;

	/*! @c true if the key is pressed. */
	bool pressed;

	/*! @c Number of ticks the key has been pressed. */
	size_t ticks_pressed;
};

/*! 
@brief Stores the callbacks and state of a keyboard.
*/
struct keyboard
{
	struct window* window;

	/*! Collection of callbacks registered with a keyboard. */
	//struct darray* callbacks;

	size_t callback_count;
	struct keyboard_callback_bundle callbacks[KEYBOARD_CALLBACK_CAPACITY];

	/*! State of all monitored keys. */
	struct keyboard_key_state key_states[KEYBOARD_KEY_COUNT];
};

/*!
@brief Allocate memory for and initialize a keyboard.

@return Pointer to the created keyboard or @c NULL on failure.
*/
struct keyboard* keyboard_create(struct window* window);

/*!
@brief Deallocate memory allocated by the keyboard as well as the keyboard
itself.

@param self Instance of keyboard.
*/
void keyboard_destroy(struct keyboard* self);

/*!
@brief Register a callback with the keyboard.

@param self Instance of keyboard.
@param callback Callback to register.
@param user User data to pass into the callback.
@see keyboard_event_callback
*/
//void keyboard_register_callback(struct keyboard* self, keyboard_callback callback, void* user);

void keyboard_push_callback(struct keyboard* self, keyboard_callback callback, void* user);

void keyboard_pop_callback(struct keyboard* self);

/*!
@brief Update the keyboard state and notify all registered callbacks of events
that have occurred.

@param self Instance of keyboard.
*/
void keyboard_tick(struct keyboard* self);

#endif
