#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>

struct shader
{
	GLuint handle;
};

/*
struct shaders
{
	struct shader cursor;
	struct shader voxel;
	struct shader interface;
	struct shader ground;
	struct shader text;
};

extern struct shaders Shaders;
*/

void shader_init(struct shader* shader);

void shader_free(struct shader* shader);

void shader_load(struct shader* shader, char* vertex_path, char* fragment_path);

#endif
