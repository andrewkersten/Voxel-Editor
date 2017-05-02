#include "colorbar.h"

#include <assert.h>
#include <stdlib.h>

bool colorbar_keyboard_callback(int key, enum keyboard_action action, void* user);

//bool colorbar_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user);

struct colorbar* colorbar_create(struct keyboard* keyboard, struct mouse* mouse)
{
	assert(keyboard);
	assert(mouse);

	struct colorbar* self = calloc(1, sizeof(*self));
	assert(self);

	self->selected_index = 0;

	self->colors[0] = (struct color) { 0.749f, 0.251f, 0.251f, 1.0f };
	self->colors[1] = (struct color) { 0.749f, 0.584f, 0.251f, 1.0f };
	self->colors[2] = (struct color) { 0.584f, 0.749f, 0.251f, 1.0f };
	self->colors[3] = (struct color) { 0.251f, 0.749f, 0.251f, 1.0f };
	self->colors[4] = (struct color) { 0.251f, 0.749f, 0.584f, 1.0f };
	self->colors[5] = (struct color) { 0.251f, 0.584f, 0.749f, 1.0f };
	self->colors[6] = (struct color) { 0.251f, 0.251f, 0.749f, 1.0f };
	self->colors[7] = (struct color) { 0.584f, 0.251f, 0.749f, 1.0f };
	self->colors[8] = (struct color) { 0.749f, 0.251f, 0.584f, 1.0f };
	self->colors[9] = (struct color) { 0.749f, 0.251f, 0.251f, 1.0f };

	keyboard_push_callback(keyboard, colorbar_keyboard_callback, self);
	//mouse_push_callback(mouse, colorbar_mouse_callback, self);

	return self;
}

void colorbar_destroy(struct colorbar* self)
{
	assert(self);

	free(self);
}

void colorbar_set(struct colorbar* self, size_t index, struct color color)
{
	assert(self);
	
	assert(index < COLORBAR_COLOR_COUNT);
	if (index >= COLORBAR_COLOR_COUNT)
	{
		return;
	}

	self->colors[index] = color;
}

bool colorbar_keyboard_callback(int key, enum keyboard_action action, void* user)
{
	struct colorbar* self = user;

	if (action == KEYBOARD_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_1:
			{
				self->selected_index = 0;
				return true;
			}
			case GLFW_KEY_2:
			{
				self->selected_index = 1;
				return true;
			}
			case GLFW_KEY_3:
			{
				self->selected_index = 2;
				return true;
			}
			case GLFW_KEY_4:
			{
				self->selected_index = 3;
				return true;
			}
			case GLFW_KEY_5:
			{
				self->selected_index = 4;
				return true;
			}
			case GLFW_KEY_6:
			{
				self->selected_index = 5;
				return true;
			}
			case GLFW_KEY_7:
			{
				self->selected_index = 6;
				return true;
			}
			case GLFW_KEY_8:
			{
				self->selected_index = 7;
				return true;
			}
			case GLFW_KEY_9:
			{
				self->selected_index = 8;
				return true;
			}
			case GLFW_KEY_0:
			{
				self->selected_index = 9;
				return true;
			}
			default:
			{
				break;
			}
		}
	}

	return false;
}

/*
bool colorbar_mouse_callback(enum mouse_action action, struct mouse* mouse, int button, void* user)
{
	return false;
}
//*/

/*
int color_selected = 0;

float colors[16 * 3];

for (int i = 0; i < 16; i++)
{
colors[i * 3 + 0] = 1.0f;
colors[i * 3 + 1] = 0.0f;
colors[i * 3 + 2] = 0.0f;
}

float colors[] = {
1.0f, 0.0f, 0.0f,
1.0f, 0.5f, 0.0f,
1.0f, 1.0f, 0.0f,
0.5f, 1.0f, 0.0f,
0.0f, 1.0f, 0.5f,
0.0f, 1.0f, 1.0f,
0.0f, 0.5f, 1.0f,
0.0f, 0.0f, 1.0f,
0.5f, 0.0f, 1.0f,
1.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.5f,
1.0f, 1.0f, 1.0f,
0.75f, 0.75f, 0.75f,
0.5f, 0.5f, 0.5f,
0.25f, 0.25f, 0.25f,
0.0f, 0.0f, 0.0f,
};


colorbar_update(colors);
//*/
//colorbar_update(colors);

// ----------------------------------------

// Color Bar
/*
glDisable(GL_DEPTH_TEST);

glUseProgram(Shaders.interface.handle);

glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&projection_2d);

modelview = GLKMatrix4Multiply(view, GLKMatrix4Identity);

glUniformMatrix4fv(4, 1, GL_FALSE, (const GLfloat*)&modelview);

glBindVertexArray(ColorBar.vao);

glDrawArrays(GL_TRIANGLES, 0, COLORBAR_ELEMENT_COUNT);

glEnable(GL_DEPTH_TEST);
*/

/*
struct colorbar ColorBar;

void colorbar_init(void)
{
	ColorBar.selected = 0;

	//glfwSetScrollCallback(Window.window, colorbar_scroll);

	glGenVertexArrays(1, &ColorBar.vao);
	glBindVertexArray(ColorBar.vao);

	glGenBuffers(1, &ColorBar.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, ColorBar.vbo);
	glBufferData(GL_ARRAY_BUFFER, COLORBAR_VERTEX_COUNT * sizeof(struct vertex_colorbar), NULL, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_colorbar), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_colorbar), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void colorbar_update(float* colors)
{
	float left = 418.0f;
	float right = left + 34.0f;
	float top = 843.0f;
	float bottom = top + 34.0f;

	for (int i = 0; i < COLORBAR_COLOR_COUNT; i++)
	{
		if (i == ColorBar.selected)
		{
			top -= 2;
			bottom += 2;
			left -= 2;
			right += 2;
		}

		ColorBar.vertices[i * 6 + 0].position.x = left;
		ColorBar.vertices[i * 6 + 0].position.y = bottom;
		ColorBar.vertices[i * 6 + 0].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 0].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 0].color.b = 0.0f;

		ColorBar.vertices[i * 6 + 1].position.x = left;
		ColorBar.vertices[i * 6 + 1].position.y = top;
		ColorBar.vertices[i * 6 + 1].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 1].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 1].color.b = 0.0f;

		ColorBar.vertices[i * 6 + 2].position.x = right;
		ColorBar.vertices[i * 6 + 2].position.y = top;
		ColorBar.vertices[i * 6 + 2].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 2].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 2].color.b = 0.0f;

		ColorBar.vertices[i * 6 + 3].position.x = left;
		ColorBar.vertices[i * 6 + 3].position.y = bottom;
		ColorBar.vertices[i * 6 + 3].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 3].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 3].color.b = 0.0f;

		ColorBar.vertices[i * 6 + 4].position.x = right;
		ColorBar.vertices[i * 6 + 4].position.y = top;
		ColorBar.vertices[i * 6 + 4].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 4].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 4].color.b = 0.0f;
	
		ColorBar.vertices[i * 6 + 5].position.x = right;
		ColorBar.vertices[i * 6 + 5].position.y = bottom;
		ColorBar.vertices[i * 6 + 5].color.r = 0.0f;
		ColorBar.vertices[i * 6 + 5].color.g = 0.0f;
		ColorBar.vertices[i * 6 + 5].color.b = 0.0f;

		if (i == ColorBar.selected)
		{
			top += 2;
			bottom -= 2;
			left += 2;
			right -= 2;
		}

		left += 38.0f;
		right += 38.0f;
	}

	left = 419.0f;
	right = left + 32.0f;
	top = 844.0f;
	bottom = top + 32.0f;

	for (int i = 0; i < COLORBAR_COLOR_COUNT; i++)
	{
		float r = colors[i * 3 + 0];
		float g = colors[i * 3 + 1];
		float b = colors[i * 3 + 2];

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 0].position.x = left;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 0].position.y = bottom;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 0].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 0].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 0].color.b = b;

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 1].position.x = left;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 1].position.y = top;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 1].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 1].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 1].color.b = b;

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 2].position.x = right;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 2].position.y = top;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 2].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 2].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 2].color.b = b;

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 3].position.x = left;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 3].position.y = bottom;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 3].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 3].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 3].color.b = b;

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 4].position.x = right;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 4].position.y = top;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 4].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 4].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 4].color.b = b;

		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 5].position.x = right;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 5].position.y = bottom;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 5].color.r = r;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 5].color.g = g;
		ColorBar.vertices[COLORBAR_COLOR_COUNT * 6 + i * 6 + 5].color.b = b;

		left += 38.0f;
		right += 38.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, ColorBar.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, COLORBAR_VERTEX_COUNT * sizeof(struct vertex_colorbar), ColorBar.vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void colorbar_scroll(GLFWwindow* window, double x, double y)
{
	int scroll = (int)y;

	ColorBar.selected -= y;

	while (ColorBar.selected < 0)
	{
		ColorBar.selected += COLORBAR_COLOR_COUNT;
	}

	while (ColorBar.selected >= COLORBAR_COLOR_COUNT)
	{
		ColorBar.selected -= COLORBAR_COLOR_COUNT;
	}
}
*/
