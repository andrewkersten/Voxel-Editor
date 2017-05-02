#include "keyboard.h"

#include <assert.h>
#include <stdlib.h>

// forward declare private function(s)

void keyboard_trigger_event(struct keyboard* self, int key, enum keyboard_action action);

struct keyboard* keyboard_create(struct window* window)
{
	assert(window);

	struct keyboard* self = calloc(1, sizeof(*self));

	//self->callbacks = darray_create(sizeof(struct keyboard_callback_bundle), 16);

	self->window = window;

	self->callback_count = 0;

	return self;
}

void keyboard_destroy(struct keyboard* self)
{
	assert(self);

	//darray_destroy(self->callbacks);

	free(self);
}

/*
void keyboard_register_callback(struct keyboard* self, keyboard_callback callback, void* user)
{
	assert(self);
	assert(callback);

	struct keyboard_callback_bundle bundle;

	bundle.callback = callback;
	bundle.user = user;

	darray_append(self->callbacks, &bundle);
}
*/

void keyboard_push_callback(struct keyboard* self, keyboard_callback callback, void* user)
{
	assert(self);
	assert(callback);

	if (self->callback_count == KEYBOARD_CALLBACK_CAPACITY)
	{
		assert(false);
		return;
	}

	self->callbacks[self->callback_count].callback = callback;
	self->callbacks[self->callback_count].user = user;

	self->callback_count++;
}

void keyboard_pop_callback(struct keyboard* self)
{
	assert(self);

	if (self->callback_count == 0)
	{
		assert(false);
		return;
	}

	self->callback_count--;
}

void keyboard_tick(struct keyboard* self)
{
	assert(self);

	for (int i = 0; i < KEYBOARD_KEY_COUNT; i++)
	{
		self->key_states[i].released = false;

		if (window_get_key_state(self->window, i))
		{
			if (self->key_states[i].ticks_pressed == 0)
			{
				keyboard_trigger_event(self, i, KEYBOARD_PRESS);
			}
			else
			{
				keyboard_trigger_event(self, i, KEYBOARD_HOLD);

				if (self->key_states[i].ticks_pressed % 10 == 0)
				{
					keyboard_trigger_event(self, i, KEYBOARD_REPEAT);
				}
			}

			self->key_states[i].pressed = true;
			self->key_states[i].ticks_pressed++;
		}
		else
		{
			if (self->key_states[i].pressed)
			{
				keyboard_trigger_event(self, i, KEYBOARD_RELEASE);

				self->key_states[i].released = true;
			}

			self->key_states[i].pressed = false;
			self->key_states[i].ticks_pressed = 0;
		}
	}
}

/*!
@brief Notify all registered callbacks that an event has occurred.

@param self Instance of keyboard.
@param key Key that triggered event.
@param action Action that has occurred.
*/
static void keyboard_trigger_event(struct keyboard* self, int key, enum keyboard_action action)
{
	assert(self);

	/*
	darray_cursor_set(self->callbacks, 0);

	struct keyboard_callback_bundle* bundle = NULL;

	while (bundle = darray_next(self->callbacks), bundle)
	{
		bundle->callback(key, action, bundle->user);
	}
	*/

	for (int i = self->callback_count; i > 0; i--)
	{
		struct keyboard_callback_bundle bundle = self->callbacks[i - 1];

		if (bundle.callback(key, action, bundle.user))
		{
			break;
		}
	}

	/*
	if (self->callback_count > 0)
	{
		struct keyboard_callback_bundle bundle = self->callbacks[self->callback_count - 1];

		bundle.callback(key, action, bundle.user);
	}
	*/
}
