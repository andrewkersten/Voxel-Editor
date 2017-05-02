#pragma once

#include <stdbool.h>

#include "camera.h"
#include "colorbar.h"
#include "keyboard.h"
#include "mouse.h"
#include "panel.h"
#include "transform.h"
#include "volume.h"

struct editor
{
	struct mouse* mouse;

	struct camera* camera;
	struct panel* panel;
	struct colorbar* colorbar;

	struct volume volume;
	struct transform transform;

	bool has_focus;
	bool can_focus;

	float selector_r;
	float selector_g;
	float selector_b;
};

struct editor* editor_create(struct window* window, struct keyboard* keyboard, struct mouse* mouse);

void editor_destroy(struct editor* self);
