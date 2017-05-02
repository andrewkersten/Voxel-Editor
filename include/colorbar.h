#ifndef __COLORBAR_H__
#define __COLORBAR_H__

#include "color.h"
#include "keyboard.h"
#include "mouse.h"

#define COLORBAR_COLOR_COUNT 10

/*
#include <GL/glew.h>

#include "window.h"

#define COLORBAR_COLOR_COUNT 16
#define COLORBAR_VERTEX_COUNT 16 * 2 * 6
#define COLORBAR_ELEMENT_COUNT 16 * 2 * 6

struct vertex_colorbar
{
	union
	{
		struct
		{
			GLfloat x;
			GLfloat y;
		} position;

		GLfloat position_buffer[2];
	};

	union
	{
		struct
		{
			GLfloat r;
			GLfloat g;
			GLfloat b;
		} color;

		GLfloat color_buffer[3];
	};
};
*/

struct colorbar
{
	/*
	GLuint vao;
	GLuint vbo;

	struct vertex_colorbar vertices[COLORBAR_VERTEX_COUNT];
	*/

	struct color colors[COLORBAR_COLOR_COUNT];

	size_t selected_index;
};

struct colorbar* colorbar_create(struct keyboard* keyboard, struct mouse* mouse);

void colorbar_destroy(struct colorbar* self);

void colorbar_set(struct colorbar* self, size_t index, struct color color);

//extern struct colorbar ColorBar;

//void colorbar_init(void);

//void colorbar_update(float* colors);

//void colorbar_scroll(GLFWwindow* window, double x, double y);

#endif
