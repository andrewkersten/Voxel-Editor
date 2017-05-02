#include "engine.h"

#include "lodepng.h"
#include "renderer_gl.h"

#include <assert.h>
#include <time.h>

bool screenshot_listener(int key, enum keyboard_action action, struct engine* engine);

struct engine* engine_create()
{
	struct engine* self = calloc(1, sizeof(*self));

	self->window = window_create();
	self->keyboard = keyboard_create(self->window);
	self->mouse = mouse_create(self->window);

	keyboard_push_callback(self->keyboard, screenshot_listener, self);

	self->editor = editor_create(self->window, self->keyboard, self->mouse);

	self->renderer = (struct renderer*)renderer_gl_create(self->window);

	return self;
}

void engine_loop(struct engine* self)
{
	assert(self);

	while (!window_should_close(self->window))
	{
		window_tick(self->window);
		keyboard_tick(self->keyboard);
		mouse_tick(self->mouse);
		
		self->renderer->clear(self->renderer);

		self->renderer->render_editor(self->renderer, self->editor);

		/*
		self->renderer->render_text(self->renderer, "Version 0.1.26", 10, self->window->height - 10);
		
		char hsl_string_buffer[128];
		snprintf(hsl_string_buffer, 128, "H: %f S: %f L: %f", self->editor->panel->selector_H, self->editor->panel->selector_S, self->editor->panel->selector_L);
		self->renderer->render_text(self->renderer, hsl_string_buffer, 10, self->window->height - 30);

		char rgb_string_buffer[128];
		snprintf(rgb_string_buffer, 128, "R: %f G: %f B: %f", self->editor->selector_r, self->editor->selector_g, self->editor->selector_b);
		self->renderer->render_text(self->renderer, rgb_string_buffer, 10, self->window->height - 50);
		//*/

		window_swap_buffers(self->window);
	}
}

bool screenshot_listener(int key, enum keyboard_action action, struct engine* self)
{
	assert(self);

	if (action == KEYBOARD_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_F2:
			{
				size_t width = 0;
				size_t height = 0;

				unsigned char* data = self->renderer->screenshot(self->renderer, &width, &height);

				if (data == NULL)
				{
					break;
				}

				char filename[30];

				time_t rawtime;
				struct tm * timeinfo;

				time(&rawtime);
				timeinfo = localtime(&rawtime);

				strftime(filename, 30, "screenshots/%Y%j%H%M%S.png\0", timeinfo);

				lodepng_encode24_file(filename, data, width, height);

				free(data);

				return true;
			}
		}
	}

	return false;
}
