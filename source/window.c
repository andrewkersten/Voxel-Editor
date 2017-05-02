#include "window.h"

#include "utility.h"

#include <assert.h>

struct window* window_create(void)
{
	struct window* window = calloc(1, sizeof(*window));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window->glfw = glfwCreateWindow(WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, WINDOW_TITLE, NULL, NULL);
	glfwSetWindowUserPointer(window->glfw, window);

	window->width = WINDOW_DEFAULT_WIDTH;
	window->width_half = WINDOW_DEFAULT_WIDTH / 2;
	window->height = WINDOW_DEFAULT_HEIGHT;
	window->height_half = WINDOW_DEFAULT_HEIGHT / 2;

	const GLFWvidmode* monitor_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwSetWindowPos(window->glfw, monitor_video_mode->width / 2 - window->width_half, monitor_video_mode->height / 2 - window->height_half);

	glfwMakeContextCurrent(window->glfw);

	// Commented out = vsync on
	//glfwSwapInterval(0);

	return window;
}

bool window_should_close(struct window* self)
{
	assert(self);

	return glfwWindowShouldClose(self->glfw);
}

void window_tick(struct window* self)
{
	assert(self);

	glfwPollEvents();
}

void window_swap_buffers(struct window* self)
{
	assert(self);

	glfwSwapBuffers(self->glfw);
}

void window_set_cursor_visible(struct window* self, bool visible)
{
	if (visible)
	{
		glfwSetInputMode(self->glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(self->glfw, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
}

bool window_get_cursor_visible(struct window* self)
{
	assert(self);

	if (glfwGetInputMode(self->glfw, GLFW_CURSOR) == GLFW_CURSOR_HIDDEN)
	{
		return false;
	}

	return true;
}

void window_center_cursor(struct window* self)
{
	glfwSetCursorPos(self->glfw, self->width / 2, self->height / 2);
}

bool window_get_key_state(struct window* self, int key)
{
	return glfwGetKey(self->glfw, key) == GLFW_PRESS;
}

void window_get_cursor_position(struct window* self, double* x, double* y)
{
	assert(self);
	assert(x);
	assert(y);

	glfwGetCursorPos(self->glfw, x, y);
}

bool window_get_mouse_button_state(struct window* self, int button)
{
	assert(self);

	return glfwGetMouseButton(self->glfw, button) == GLFW_PRESS;
}
