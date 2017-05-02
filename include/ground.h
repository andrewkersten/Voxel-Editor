#ifndef __GROUND_H__
#define __GROUND_H__

#include <GL/glew.h>

#include "transform.h"

#define GROUND_VERTEX_COUNT 6

struct vertex_ground
{
	union
	{
		struct
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;
		} position;

		GLfloat position_buffer[3];
	};

	union
	{
		struct
		{
			GLfloat u;
			GLfloat v;
		} texcoord;

		GLfloat texcoord_buffer[2];
	};
};

struct ground
{
	GLuint vao;
	GLuint vbo;

	struct transform transform;
};

extern struct ground Ground;

void ground_init(void);

#endif
