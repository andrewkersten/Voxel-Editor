#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <stdbool.h>

#define WINDOW_DEFAULT_WIDTH 1440
#define WINDOW_DEFAULT_HEIGHT 900
//#define WINDOW_HALF_WIDTH WINDOW_WIDTH / 2
//#define WINDOW_HALF_HEIGHT WINDOW_HEIGHT / 2
#define WINDOW_TITLE "VEdit"

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

struct window
{
	GLFWwindow* glfw;

	int width;
	int width_half;
	int height;
	int height_half;
};

struct window* window_create(void);

bool window_should_close(struct window* self);

void window_tick(struct window* self);

void window_swap_buffers(struct window* self);

void window_set_cursor_visible(struct window* self, bool visible);

bool window_get_cursor_visible(struct window* self);

void window_center_cursor(struct window* self);

bool window_get_key_state(struct window* self, int key);

void window_get_cursor_position(struct window* self, double* x, double* y);

bool window_get_mouse_button_state(struct window* self, int button);

#endif

