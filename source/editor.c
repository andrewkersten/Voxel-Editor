#include "editor.h"

#include <assert.h>
#include <stdlib.h>

void create_volume(struct editor* self);

typedef bool(*raytrace_callback)(struct editor* self, int idx, int edx, GLKVector3 entered);

void raytrace(struct editor* self, raytrace_callback callback);

bool raytrace_break(struct editor* self, int idx, int edx, GLKVector3 entered);

bool raytrace_place(struct editor* self, int idx, int edx, GLKVector3 entered);

bool editor_keyboard_callback(int key, enum keyboard_action action, void* user);

//bool editor_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user);

bool editor_mouse_press_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

bool editor_mouse_release_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user);

struct editor* editor_create(struct window* window, struct keyboard* keyboard, struct mouse* mouse)
{
	assert(keyboard);
	assert(mouse);

	struct editor* self = calloc(1, sizeof(*self));

	assert(self);

	self->has_focus = true;

	self->mouse = mouse;
	mouse_center_cursor(mouse);
	mouse_set_cursor_visible(mouse, false);

	self->camera = camera_create(window, keyboard, mouse);
	camera_set_position(self->camera, GLKVector3Make(1000.0f, 1010.0f, 1000.0f));

	self->panel = panel_create();

	self->colorbar = colorbar_create(keyboard, mouse);

	create_volume(self);

	keyboard_push_callback(keyboard, editor_keyboard_callback, self);
	mouse_push_press_callback(mouse, editor_mouse_press_cb, self);
	mouse_push_release_callback(mouse, editor_mouse_release_cb, self);
	//mouse_push_callback(mouse, editor_mouse_callback, self);

	return self;
}

void editor_destroy(struct editor* self)
{
	assert(self);

	free(self);

	self = NULL;
}

void create_volume(struct editor* self)
{
	volume_init(&self->volume);

	transform_init(&self->transform);
	self->transform.translation = GLKVector3Make(1000.0f, 1000.0f, 1000.0f);
	transform_calc(&self->transform);

	for (size_t i = 0; i < self->volume.volume_outer; i++)
	{
		int y = (i / (self->volume.length_outer_x * self->volume.length_outer_z));
		int z = ((i % (self->volume.length_outer_x * self->volume.length_outer_z)) / self->volume.length_outer_x);
		int x = (i % self->volume.length_outer_x);

		if (x == 0 || x == (self->volume.length_outer_x - 1) || y == 0 || y == (self->volume.length_outer_y - 1) || z == 0 || (z == self->volume.length_outer_z - 1))
		{
			self->volume.blocks[i] = false;
		}
		else
		{
			if (y == 1)
			{
				self->volume.blocks[i] = true;
			}
			else
			{
				self->volume.blocks[i] = false;
			}
		}
	}

	for (size_t i = 0; i < self->volume.volume_inner; i++)
	{
		self->volume.colors[i * 3 + 0] = 255;
		self->volume.colors[i * 3 + 1] = 255;
		self->volume.colors[i * 3 + 2] = 255;
	}

	volume_mesh(&self->volume);
}

bool editor_keyboard_callback(int key, enum keyboard_action action, void* user)
{
	assert(user);

	struct editor* self = user;

	if (self->has_focus)
	{
		if (action == KEYBOARD_RELEASE)
		{
			if (key == GLFW_KEY_ESCAPE)
			{
				self->has_focus = false;

				mouse_set_cursor_visible(self->mouse, true);
				panel_push_callbacks(self->panel, self->mouse);
				//mouse_push_callback(self->mouse, panel_mouse_callback, self->panel);
				
				return true;
			}
		}
	}

	return false;
}

bool editor_mouse_press_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user)
{
	assert(mouse);
	assert(button);
	assert(user);

	struct editor* self = user;

	if (button->type == MOUSE_BUTTON_LEFT)
	{
		if (self->has_focus == false)
		{
			if (mouse->x > 280)
			{
				self->can_focus = true;
			}
		}
	}
}

bool editor_mouse_release_cb(const struct mouse* mouse, const struct mouse_button_state* button, void* user)
{
	assert(mouse);
	assert(button);
	assert(user);

	struct editor* self = user;

	if (button->type == MOUSE_BUTTON_LEFT)
	{
		if (self->has_focus)
		{
			raytrace(self, raytrace_break);
		}
		else
		{
			if (self->can_focus)
			{
				if (mouse->x > 280)
				{
					self->has_focus = true;

					//mouse_pop_callback(self->mouse);
					panel_pop_callbacks(self->panel, self->mouse);
					mouse_set_cursor_visible(self->mouse, false);
				}
			}

			self->can_focus = false;

			return true;
		}
	}
	else if (button->type == MOUSE_BUTTON_RIGHT)
	{
		if (self->has_focus)
		{
			raytrace(self, raytrace_place);
			return true;
		}
	}
}

/*
bool editor_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user)
{
	assert(user);

	struct editor* self = user;

	if (action == MOUSE_BUTTON_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_1)
		{
			if (self->has_focus == false)
			{
				if (mouse->x > 280)
				{
					self->can_focus = true;
				}
			}
		}
	}
	else if (action == MOUSE_BUTTON_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_1)
		{
			if (self->has_focus)
			{
				raytrace(self, raytrace_break);
			}
			else
			{
				if (self->can_focus)
				{
					if (mouse->x > 280)
					{
						self->has_focus = true;

						//mouse_pop_callback(self->mouse);
						panel_pop_callbacks(self->panel, self->mouse);
						mouse_set_cursor_visible(self->mouse, false);
					}
				}
				
				self->can_focus = false;

				return true;
			}
		}
		else if (button == GLFW_MOUSE_BUTTON_2)
		{
			if (self->has_focus)
			{
				raytrace(self, raytrace_place);
				return true;
			}
		}
	}

	return false;
}
//*/

bool raytrace_break(struct editor* self, int idx, int edx, GLKVector3 entered)
{
	assert(self);

	self->volume.blocks[edx] = false;
	volume_mesh(&self->volume);

	return true;
}

bool raytrace_place(struct editor* self, int idx, int edx, GLKVector3 entered)
{
	assert(self);

	int bdx = get_index_relative_to(edx, self->volume.length_outer_x, self->volume.length_outer_z, entered.x, entered.y, entered.z);

	self->volume.blocks[bdx] = true;

	int cdx = get_index_relative_to2(idx, &self->volume, entered.x, entered.y, entered.z);

	self->volume.colors[cdx * 3 + 0] = self->colorbar->colors[self->colorbar->selected_index].r * 255; //self->selector_r * 255;
	self->volume.colors[cdx * 3 + 1] = self->colorbar->colors[self->colorbar->selected_index].g * 255; //self->selector_g * 255;
	self->volume.colors[cdx * 3 + 2] = self->colorbar->colors[self->colorbar->selected_index].b * 255; //self->selector_b * 255;

	volume_mesh(&self->volume);

	return true;
}

static int signof(float f)
{
	if (f > 0)
	{
		return 1;
	}
	else if (f < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

static float calcmax(float origin, float direction)
{
	if (direction < 0)
	{
		return (floor(origin) - origin) / direction;
	}
	else
	{
		return (floor(origin) + 1 - origin) / direction;
	}
}

void raytrace(struct editor* self, raytrace_callback callback)
{
	assert(self);

	float rox = self->camera->position.x;
	float roy = self->camera->position.y;
	float roz = self->camera->position.z;

	int vx = (int)rox - 1000;
	int vy = (int)roy - 1000;
	int vz = (int)roz - 1000;

	float rdx = self->camera->direction.x;
	float rdy = self->camera->direction.y;
	float rdz = self->camera->direction.z;

	int stepx = signof(rdx);
	int stepy = signof(rdy);
	int stepz = signof(rdz);

	float tdeltax = (float)stepx / rdx;
	float tdeltay = (float)stepy / rdy;
	float tdeltaz = (float)stepz / rdz;

	//float tmaxx = tdeltax * (1.0 - mod(rox));
	//float tmaxy = tdeltay * (1.0 - mod(roy));
	//float tmaxz = tdeltaz * (1.0 - mod(roz));

	float tmaxx = calcmax(rox, rdx);
	float tmaxy = calcmax(roy, rdy);
	float tmaxz = calcmax(roz, rdz);

	int facex = 0;
	int facey = 0;
	int facez = 0;

	float radius = 100;
	radius /= sqrtf(rdx * rdx + rdy * rdy + rdz * rdz);

	while (true)
	{
		if (vx >= 0 && vx < self->volume.length_inner_x && vy >= 0 && vy < self->volume.length_inner_y && vz >= 0 && vz < self->volume.length_inner_z)
		{
			int idx = get_index(vx, vy, vz, &self->volume);
			int edx = get_expanded_index(idx, &self->volume);

			GLKVector3 entered = GLKVector3Make(facex, facey, facez);

			if (self->volume.blocks[edx])
			{
				/*
				if (self->mouse->button_states[GLFW_MOUSE_BUTTON_1].released)
				{
					self->volume.blocks[edx] = false;
				}
				else if (self->mouse->button_states[GLFW_MOUSE_BUTTON_2].released)
				{
					int bdx = get_index_relative_to(edx, self->volume.length_outer_x, self->volume.length_outer_z, facex, facey, facez);

					self->volume.blocks[bdx] = true;

					int cdx = get_index_relative_to2(idx, &self->volume, facex, facey, facez);

					self->volume.colors[cdx * 3 + 0] = 255;//colors[ColorBar.selected * 3 + 0] * 255;
					self->volume.colors[cdx * 3 + 1] = 0;// colors[ColorBar.selected * 3 + 1] * 255;
					self->volume.colors[cdx * 3 + 2] = 0;// colors[ColorBar.selected * 3 + 2] * 255;
				}
				else if (self->mouse->button_states[GLFW_MOUSE_BUTTON_3].released)
				{
					colors[ColorBar.selected * 3 + 0] = (float)volume.colors[idx * 3 + 0] / 255.0f;
					colors[ColorBar.selected * 3 + 1] = (float)volume.colors[idx * 3 + 1] / 255.0f;
					colors[ColorBar.selected * 3 + 2] = (float)volume.colors[idx * 3 + 2] / 255.0f;
				}

				//volume.colors[idx * 3 + 0] = 255;
				//volume.colors[idx * 3 + 1] = 0;
				//volume.colors[idx * 3 + 2] = 0;

				volume_mesh(&self->volume);

				break;
				*/

				if (callback(self, idx, edx, entered))
				{
					break;
				}
			}
		}

		if (tmaxx < tmaxy)
		{
			if (tmaxx < tmaxz)
			{
				vx += stepx;
				tmaxx += tdeltax;

				if (tmaxx > radius)
				{
					break;
				}

				facex = -stepx;
				facey = 0;
				facez = 0;
			}
			else
			{
				vz += stepz;
				tmaxz += tdeltaz;

				if (tmaxz > radius)
				{
					break;
				}

				facex = 0;
				facey = 0;
				facez = -stepz;
			}
		}
		else
		{
			if (tmaxy < tmaxz)
			{
				vy += stepy;
				tmaxy += tdeltay;

				if (tmaxy > radius)
				{
					break;
				}

				facex = 0;
				facey = -stepy;
				facez = 0;
			}
			else
			{
				vz += stepz;
				tmaxz += tdeltaz;

				if (tmaxz > radius)
				{
					break;
				}

				facex = 0;
				facey = 0;
				facez = -stepz;
			}
		}
	}
}

/*
if (self->keyboard->key_states[GLFW_KEY_F1].released)
{
for (int x = 0; x < volume.length_outer_x; x++)
{
for (int z = 0; z < volume.length_outer_z; z++)
{
volume.blocks[get_expanded_index2(&volume, x, 1, z)] = false;
}
}

volume_mesh(&volume);
volume_export(&volume);

for (int x = 0; x < volume.length_outer_x; x++)
{
for (int z = 0; z < volume.length_outer_z; z++)
{
volume.blocks[get_expanded_index2(&volume, x, 1, z)] = true;
}
}

volume_mesh(&volume);
}
*/

/*
if (self->keyboard->key_states[GLFW_KEY_F2].released)
{
size_t data_size = 3 * WINDOW_WIDTH * WINDOW_HEIGHT;
GLubyte* data = malloc(data_size);

if (data == NULL)
{
allocation_failure();
}

glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

size_t stride = 3 * WINDOW_WIDTH;
GLubyte* buffer = malloc(stride);

for (int i = 0; i < WINDOW_HEIGHT / 2; i++)
{
memcpy(buffer, data + i * stride, stride);
memcpy(data + i * stride, data + data_size - (i + 1) * stride, stride);
memcpy(data + data_size - (i + 1) * stride, buffer, stride);
}

char filename[30];

time_t rawtime;
struct tm * timeinfo;

time(&rawtime);
timeinfo = localtime(&rawtime);

strftime(filename, 30, "screenshots/%Y%j%H%M%S.png\0", timeinfo);

lodepng_encode24_file(filename, data, WINDOW_WIDTH, WINDOW_HEIGHT);

free(buffer);
free(data);
}
*/
