#pragma once

#include "mouse.h"
#include "rect.h"

struct panel
{
	float selector_H;
	float selector_S;
	float selector_L;

	struct rect rect_hue_slider;
	struct rect rect_selector;
	struct rect rect_hotbar;

	bool selecting_hue;
	bool selecting_color;
};

struct panel* panel_create(void);

void panel_destroy(struct panel* self);

void panel_push_callbacks(struct panel* self, struct mouse* mouse);

void panel_pop_callbacks(struct panel* self, struct mouse* mouse);

//bool panel_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user);
