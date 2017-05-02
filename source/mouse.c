#include "mouse.h"

#include <assert.h>
#include <stdlib.h.>

//void mouse_trigger_event(struct mouse* self, enum mouse_action action, int button);

void mouse_update_button(struct mouse* self, struct mouse_button_state* state);

void mouse_trigger_move(struct mouse* self);

void mouse_trigger_drag(struct mouse* self);

void mouse_trigger_press(struct mouse* self, struct mouse_button_state* state);

void mouse_trigger_hold(struct mouse* self, struct mouse_button_state* state);

void mouse_trigger_release(struct mouse* self, struct mouse_button_state* state);

struct mouse* mouse_create(struct window* window)
{
	assert(window);

	struct mouse* self = calloc(1, sizeof(*self));

	self->window = window;

	self->state_left.type = MOUSE_BUTTON_LEFT;
	self->state_right.type = MOUSE_BUTTON_RIGHT;
	self->state_middle.type = MOUSE_BUTTON_MIDDLE;

	self->move_callback_count = 0;
	self->drag_callback_count = 0;
	self->press_callback_count = 0;
	self->hold_callback_count = 0;
	self->release_callback_count = 0;

	self->x = 0.0;
	self->y = 0.0;

	/*
	self->callback_count = 0;

	self->x_prev = 0.0;
	self->y_prev = 0.0;
	//*/

	return self;
}

void mouse_destroy(struct mouse* self)
{
	assert(self);

	//darray_destroy(self->callbacks);

	free(self);
}

/*
void mouse_register_callback(struct mouse* self, mouse_callback callback, void* user)
{
	assert(self);
	assert(callback);

	struct mouse_callback_bundle bundle;

	bundle.callback = callback;
	bundle.user = user;

	darray_append(self->callbacks, &bundle);
}
*/

void mouse_push_move_callback(struct mouse* self, mouse_move_callback callback, void* user)
{
	assert(self);
	//assert(callback);

	if (self->move_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->move_callbacks[self->move_callback_count].move_callback = callback;
	self->move_callbacks[self->move_callback_count].user = user;

	self->move_callback_count++;
}

void mouse_push_drag_callback(struct mouse* self, mouse_drag_callback callback, void* user)
{
	assert(self);
	//assert(callback);

	if (self->drag_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->drag_callbacks[self->drag_callback_count].drag_callback = callback;
	self->drag_callbacks[self->drag_callback_count].user = user;

	self->drag_callback_count++;
}

void mouse_push_press_callback(struct mouse* self, mouse_press_callback callback, void* user)
{
	assert(self);
	//assert(callback);

	if (self->press_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->press_callbacks[self->press_callback_count].press_callback = callback;
	self->press_callbacks[self->press_callback_count].user = user;

	self->press_callback_count++;
}

void mouse_push_hold_callback(struct mouse* self, mouse_hold_callback callback, void* user)
{
	assert(self);
	//assert(callback);

	if (self->hold_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->hold_callbacks[self->hold_callback_count].hold_callback = callback;
	self->hold_callbacks[self->hold_callback_count].user = user;

	self->hold_callback_count++;
}

void mouse_push_release_callback(struct mouse* self, mouse_release_callback callback, void* user)
{
	assert(self);
	//assert(callback);

	if (self->release_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->release_callbacks[self->release_callback_count].release_callback = callback;
	self->release_callbacks[self->release_callback_count].user = user;

	self->release_callback_count++;
}

void mouse_push_scroll_callback(struct mouse* self, mouse_scroll_callback callback, void* user)
{
	assert(self);

	if (self->scroll_callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		return;
	}

	self->scoll_callbacks[self->scroll_callback_count].scroll_callback = callback;
	self->scoll_callbacks[self->scroll_callback_count].user = user;

	self->scroll_callback_count++;
}

void mouse_pop_move_callback(struct mouse* self)
{
	assert(self);

	if (self->move_callback_count == 0)
	{
		return;
	}

	self->move_callback_count--;
}

void mouse_pop_drag_callback(struct mouse* self)
{
	assert(self);

	if (self->drag_callback_count == 0)
	{
		return;
	}

	self->drag_callback_count--;
}

void mouse_pop_press_callback(struct mouse* self)
{
	assert(self);

	if (self->press_callback_count == 0)
	{
		return;
	}

	self->press_callback_count--;
}

void mouse_pop_hold_callback(struct mouse* self)
{
	assert(self);

	if (self->hold_callback_count == 0)
	{
		return;
	}

	self->hold_callback_count--;
}

void mouse_pop_release_callback(struct mouse* self)
{
	assert(self);

	if (self->release_callback_count == 0)
	{
		return;
	}

	self->release_callback_count--;
}

void mouse_pop_scroll_callback(struct mouse* self)
{
	assert(self);

	if (self->scroll_callback_count == 0)
	{
		return;
	}

	self->scroll_callback_count--;
}

/*
void mouse_push_callback(struct mouse* self, mouse_callback callback, void* user)
{
	assert(self);
	assert(callback);

	if (self->callback_count == MOUSE_CALLBACK_CAPACITY)
	{
		assert(false);
		return;
	}

	if (self->callback_count > 0)
	{
		self->callbacks[self->callback_count - 1].visible = window_get_cursor_visible(self->window);
	}

	self->callbacks[self->callback_count].callback = callback;
	self->callbacks[self->callback_count].user = user;

	self->callback_count++;
}

void mouse_pop_callback(struct mouse* self)
{
	assert(self);

	if (self->callback_count == 0)
	{
		assert(false);
		return;
	}

	self->callback_count--;

	if (self->callback_count > 0)
	{
		window_set_cursor_visible(self->window, self->callbacks[self->callback_count - 1].visible);
	}
}
*/

void mouse_tick(struct mouse* self)
{
	assert(self);

	/*
	self->x_prev = self->x;
	self->y_prev = self->y;

	window_get_cursor_position(self->window, &self->x, &self->y);

	if (self->x != self->x_prev || self->y != self->y_prev)
	{
		mouse_trigger_event(self, MOUSE_MOVE, -1);
	}
	//*/

	double x_temp = 0.0;
	double y_temp = 0.0;
	
	window_get_cursor_position(self->window, &x_temp, &y_temp);

	self->state_left.was_dragged = false;

	if (x_temp != self->x || y_temp != self->y)
	{
		if (self->state_left.pressed)
		{
			self->state_left.was_dragged = true;
			mouse_trigger_drag(self);
		}
		else
		{
			mouse_trigger_move(self);
		}
	}

	self->x = x_temp;
	self->y = y_temp;

	mouse_update_button(self, &self->state_left);
	mouse_update_button(self, &self->state_right);
	mouse_update_button(self, &self->state_middle);

	/*
	for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
	{
		self->button_states[i].released = false;

		if (window_get_mouse_button_state(self->window, i))
		{
			if (self->button_states[i].pressed == false)
			{
				mouse_trigger_event(self, MOUSE_BUTTON_PRESS, i);
			}

			self->button_states[i].pressed = true;
		}
		else
		{
			if (self->button_states[i].pressed == true)
			{
				mouse_trigger_event(self, MOUSE_BUTTON_RELEASE, i);

				self->button_states[i].released = true;
			}

			self->button_states[i].pressed = false;
		}
	}
	//*/
}

void mouse_set_cursor_visible(struct mouse* self, bool visible)
{
	assert(self);

	window_set_cursor_visible(self->window, visible);
}

void mouse_center_cursor(struct mouse* self)
{
	assert(self);

	window_center_cursor(self->window);
}

void mouse_update_button(struct mouse* self, struct mouse_button_state* state)
{
	assert(self);
	assert(state);

	state->released = false;

	if (window_get_mouse_button_state(self->window, state->type))
	{
		if (state->pressed == false)
		{
			state->pressed = true;
			state->pressed_x = self->x;
			state->pressed_y = self->y;

			mouse_trigger_press(self, state);
		}
		else
		{
			mouse_trigger_hold(self, state);
		}
	}
	else
	{
		if (state->pressed)
		{
			state->pressed = false;
			state->released_x = self->x;
			state->released_y = self->y;

			mouse_trigger_release(self, state);
		}
	}
}

void mouse_trigger_move(struct mouse* self)
{
	assert(self);

	for (size_t i = self->move_callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->move_callbacks[i - 1];

		if (bundle.move_callback == NULL)
		{
			return;
		}

		if (bundle.move_callback(self, bundle.user))
		{
			return;
		}
	}
}

void mouse_trigger_drag(struct mouse* self)
{
	assert(self);

	for (size_t i = self->drag_callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->drag_callbacks[i - 1];

		if (bundle.drag_callback == NULL)
		{
			return;
		}

		if (bundle.drag_callback(self, &self->state_left, bundle.user))
		{
			return;
		}
	}
}

void mouse_trigger_press(struct mouse* self, struct mouse_button_state* state)
{
	assert(self);
	assert(state);

	for (size_t i = self->press_callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->press_callbacks[i - 1];

		if (bundle.press_callback == NULL)
		{
			return;
		}

		if (bundle.press_callback(self, state, bundle.user))
		{
			return;
		}
	}
}

void mouse_trigger_hold(struct mouse* self, struct mouse_button_state* state)
{
	assert(self);
	assert(state);

	for (size_t i = self->hold_callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->hold_callbacks[i - 1];

		if (bundle.hold_callback == NULL)
		{
			return;
		}

		if (bundle.hold_callback(self, state, bundle.user))
		{
			return;
		}
	}
}

void mouse_trigger_release(struct mouse* self, struct mouse_button_state* state)
{
	assert(self);

	for (size_t i = self->release_callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->release_callbacks[i - 1];

		if (bundle.release_callback == NULL)
		{
			return;
		}

		if (bundle.release_callback(self, state, bundle.user))
		{
			return;
		}
	}
}

/*
static void mouse_trigger_event(struct mouse* self, enum mouse_action action, int button)
{
	assert(self);

	for (int i = self->callback_count; i > 0; i--)
	{
		struct mouse_callback_bundle bundle = self->callbacks[i - 1];

		if (bundle.callback(action, self, button, bundle.user))
		{
			break;
		}
	}
}
*/
