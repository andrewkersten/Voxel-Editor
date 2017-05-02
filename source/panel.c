#include "panel.h"

#include <assert.h>
#include <stdlib.h>

bool panel_mouse_press_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

bool panel_mouse_release_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

bool panel_mouse_drag_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

struct panel* panel_create(void)
{
	struct panel* self = calloc(1, sizeof(*self));

	assert(self);

	self->selector_H = 0.0f;
	self->selector_S = 0.5f;
	self->selector_L = 0.5f;

	self->selecting_color = false;
	self->selecting_hue = false;

	rect_set(&self->rect_selector, 20, 20, 240, 240);
	rect_set(&self->rect_hue_slider, 20, 280, 240, 40);
	rect_set(&self->rect_hotbar, 20, 400, 239, 14);

	return self;
}

void panel_destroy(struct panel* self)
{
	assert(self);

	free(self);

	self = NULL;
}

void panel_push_callbacks(struct panel* self, struct mouse* mouse)
{
	assert(self);
	assert(mouse);

	mouse_push_press_callback(mouse, panel_mouse_press_cb, self);
	mouse_push_release_callback(mouse, panel_mouse_release_cb, self);
	mouse_push_drag_callback(mouse, panel_mouse_drag_cb, self);

	mouse_push_move_callback(mouse, NULL, NULL);
	mouse_push_hold_callback(mouse, NULL, NULL);
}

void panel_pop_callbacks(struct panel* self, struct mouse* mouse)
{
	assert(self);

	mouse_pop_press_callback(mouse);
	mouse_pop_release_callback(mouse);
	mouse_pop_drag_callback(mouse);

	mouse_pop_move_callback(mouse);
	mouse_pop_hold_callback(mouse);
}

bool panel_mouse_press_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user)
{
	struct panel* self = user;

	if (button->type == MOUSE_BUTTON_LEFT)
	{
		if (rect_contains(&self->rect_hue_slider, mouse->x, mouse->y))
		{
			self->selector_H = (float)(mouse->x - self->rect_hue_slider.x) / 240.0f;
		}
		else if (rect_contains(&self->rect_selector, mouse->x, mouse->y))
		{
			self->selector_S = (float)(mouse->x - self->rect_selector.x) / 240.0f;
			self->selector_L = 1.0f - (float)(mouse->y - self->rect_selector.y) / 240.0f;
		}

		if (mouse->x < 280)
		{
			return true;
		}
	}

	return false;
}

bool panel_mouse_release_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user)
{
	struct panel* self = user;

	double mx = mouse->x;
	double my = mouse->y;

	if (button->type == MOUSE_BUTTON_LEFT)
	{
		if (rect_contains(&self->rect_hue_slider, mx, my))
		{
			self->selector_H = (float)(mx - self->rect_hue_slider.x) / 240.0f;
		}
		else if (rect_contains(&self->rect_selector, mx, my))
		{
			self->selector_S = (float)(mx - self->rect_selector.x) / 240.0f;
			self->selector_L = 1.0f - (float)(my - self->rect_selector.y) / 240.0f;
		}
		else if (rect_contains(&self->rect_hotbar, mx, my))
		{
			int xx = mx - 20;
			if (xx % 25 < 14)
			{

			}
		}

		if (mouse->x < 280)
		{
			return true;
		}
	}

	return false;
}

bool panel_mouse_drag_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user)
{
	struct panel* self = user;

	if (button->type == MOUSE_BUTTON_LEFT)
	{

	}

	return true;
}

/*
bool panel_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user)
{
	assert(user);

	struct panel* self = user;

	if (action == MOUSE_BUTTON_PRESS)
	{
		if (rect_contains(&self->rect_hue_slider, mouse->x, mouse->y))
		{
			self->selecting_hue = true;
			self->selector_H = (float)(mouse->x - self->rect_hue_slider.x) / 240.0f;
		}

		if (rect_contains(&self->rect_selector, mouse->x, mouse->y))
		{
			self->selecting_color = true;
			self->selector_S = (float)(mouse->x - self->rect_selector.x) / 240.0f;
			self->selector_L = 1.0f - (float)(mouse->y - self->rect_selector.y) / 240.0f;
		}

		if (mouse->x < 280)
		{
			return true;
		}
	}
	else if (action == MOUSE_BUTTON_RELEASE)
	{
		self->selecting_hue = false;
		self->selecting_color = false;
	}
	else if (action == MOUSE_MOVE)
	{
		if (self->selecting_hue)
		{
			double mx = mouse->x;

			if (mx < self->rect_hue_slider.x)
			{
				mx = self->rect_hue_slider.x;
			}

			if (mx > self->rect_hue_slider.x + 240)
			{
				mx = self->rect_hue_slider.x + 240;
			}

			self->selector_H = (float)(mx - self->rect_hue_slider.x) / 240.0f;
		}

		if (self->selecting_color)
		{
			double mx = mouse->x;
			double my = mouse->y;

			if (mx < self->rect_selector.x)
			{
				mx = self->rect_selector.x;
			}

			if (mx > self->rect_selector.x + 240)
			{
				mx = self->rect_selector.x + 240;
			}

			if (my < self->rect_selector.y)
			{
				my = self->rect_selector.y;
			}

			if (my > self->rect_selector.y + 240)
			{
				my = self->rect_selector.y + 240;
			}

			self->selector_S = (float)(mx - self->rect_selector.x) / 240.0f;
			self->selector_L = 1.0f - (float)(my - self->rect_selector.y) / 240.0f;
		}

		return true;
	}

	return false;
}
//*/
