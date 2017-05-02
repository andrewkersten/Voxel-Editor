#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <GL/glew.h>

struct vertex
{
	GLubyte x;
	GLubyte y;
	GLubyte z;
	GLubyte r;
	GLubyte g;
	GLubyte b;
	
	union
	{
		GLubyte bits;

		struct
		{
			GLubyte normal : 3;
			GLubyte barycentric : 2;
			GLubyte ambient_occlusion : 3;
		} bits_struct;
	};
};

#endif
