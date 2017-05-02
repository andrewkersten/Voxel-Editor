#pragma once

#include <GL/glew.h>

struct vertex_ui_texture
{
	union
	{
		struct
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
					GLfloat u;
					GLfloat v;
				} texcoord;

				GLfloat texcoord_buffer[2];
			};
		};

		GLfloat buffer[4];
	};
};

struct vertex_ui_color
{
	union
	{
		struct
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
					GLfloat a;
				} color;

				GLfloat color_buffer[4];
			};
		};

		GLfloat buffer[6];
	};
};
