#pragma once

#include <GL/glew.h>

#define SHADER_GL_ASSET_DIRECTORY "assets/shaders/"

struct shader_gl
{
	GLuint handle;
};

struct shader_gl* shader_gl_create(const char* vertex_shader_path, const char* fragment_shader_path);

void shader_gl_destroy(struct shader_gl* self);
