#pragma once

#include <GL/glew.h>

#define TEXTURE_GL_ASSET_DIRECTORY "assets/textures/"

struct texture_gl
{
	GLuint handle;
};

struct texture_gl* texture_gl_create(const char* path);

struct texture_gl* texture_gl_create2(int width, int height, void* data);

void texture_gl_destroy(struct texture_gl* self);
