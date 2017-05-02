#include "shader_gl.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct shader_gl* shader_gl_create(const char* vertex_shader_path, const char* fragment_shader_path)
{
	struct shader_gl* self = calloc(1, sizeof(*self));
	assert(self);

	FILE* vertex_shader_file = fopen(vertex_shader_path, "rb");
	assert(vertex_shader_file);

	fseek(vertex_shader_file, 0, SEEK_END);
	long vertex_shader_size = ftell(vertex_shader_file);
	rewind(vertex_shader_file);

	unsigned char* vertex_shader_buffer = (unsigned char*)malloc(vertex_shader_size + 1);
	assert(vertex_shader_buffer);

	size_t elements_read = fread(vertex_shader_buffer, 1, vertex_shader_size, vertex_shader_file);
	assert((long)elements_read == vertex_shader_size);

	vertex_shader_buffer[vertex_shader_size] = 0;

	fclose(vertex_shader_file);

	FILE* fragment_shader_file = fopen(fragment_shader_path, "rb");
	assert(fragment_shader_file);

	fseek(fragment_shader_file, 0, SEEK_END);
	long fragment_shader_size = ftell(fragment_shader_file);
	rewind(fragment_shader_file);

	unsigned char* fragment_shader_buffer = (unsigned char*)malloc(fragment_shader_size + 1);
	assert(fragment_shader_buffer);

	elements_read = fread(fragment_shader_buffer, 1, fragment_shader_size, fragment_shader_file);
	assert((long)elements_read == fragment_shader_size);

	fragment_shader_buffer[fragment_shader_size] = 0;

	fclose(fragment_shader_file);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader, 1, (const GLchar**)&vertex_shader_buffer, NULL);
	glShaderSource(fragment_shader, 1, (const GLchar**)&fragment_shader_buffer, NULL);

	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	GLint shdr_compile_status;

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shdr_compile_status);
	if (shdr_compile_status == GL_FALSE)
	{
		char error[4096];
		GLsizei error_length;

		glGetShaderInfoLog(vertex_shader, sizeof(error), &error_length, error);

		assert(false);
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shdr_compile_status);
	if (shdr_compile_status == GL_FALSE)
	{
		char error[4096];
		GLsizei error_length;

		glGetShaderInfoLog(fragment_shader, sizeof(error), &error_length, error);

		assert(false);
	}

	self->handle = glCreateProgram();

	glAttachShader(self->handle, vertex_shader);
	glAttachShader(self->handle, fragment_shader);

	glLinkProgram(self->handle);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	free(vertex_shader_buffer);
	free(fragment_shader_buffer);

	return self;
}

void shader_gl_destroy(struct shader_gl* self)
{
	assert(self);

	glDeleteProgram(self->handle);

	free(self);
}
