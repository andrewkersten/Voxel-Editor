#include "shader.h"

#include <stdio.h>

#include "utility.h"

//struct shaders Shaders;

void shader_init(struct shader* shader)
{
	dcheck_exit_if_null(shader);

	shader->handle = 0;
}

void shader_free(struct shader* shader)
{
	dcheck_exit_if_null(shader);

	shader->handle = 0;
}

void shader_load(struct shader* shader, char* vertex_path, char* fragment_path)
{
	dcheck_exit_if_null(shader);
	dcheck_exit_if_null(vertex_path);
	dcheck_exit_if_null(fragment_path);

	FILE* vertex_shader_file = fopen(vertex_path, "rb");
	check_exit_if_null(vertex_shader_file);

	fseek(vertex_shader_file, 0, SEEK_END);
	long vertex_shader_size = ftell(vertex_shader_file);
	rewind(vertex_shader_file);

	unsigned char* vertex_shader_buffer = (unsigned char*)malloc(vertex_shader_size + 1);
	check_exit_if_null(vertex_shader_buffer);

	size_t elements_read = fread(vertex_shader_buffer, 1, vertex_shader_size, vertex_shader_file);
	check_exit((long) elements_read == vertex_shader_size, "Vertex shader read error.\n\t%s", vertex_path);

	vertex_shader_buffer[vertex_shader_size] = 0;

	fclose(vertex_shader_file);

	FILE* fragment_shader_file = fopen(fragment_path, "rb");
	check_exit_if_null(fragment_shader_file);

	fseek(fragment_shader_file, 0, SEEK_END);
	long fragment_shader_size = ftell(fragment_shader_file);
	rewind(fragment_shader_file);

	unsigned char* fragment_shader_buffer = (unsigned char*)malloc(fragment_shader_size + 1);
	check_exit_if_null(fragment_shader_buffer);

	elements_read = fread(fragment_shader_buffer, 1, fragment_shader_size, fragment_shader_file);
	check_exit((long) elements_read == fragment_shader_size, "Fragment shader read error.\n\t%s", fragment_path);

	fragment_shader_buffer[fragment_shader_size] = 0;

	fclose(fragment_shader_file);

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_buffer, NULL);
	glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_buffer, NULL);

	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	GLint shdr_compile_status;

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shdr_compile_status);
	if (shdr_compile_status == GL_FALSE)
	{
		char error[4096];
		GLsizei error_length;

		glGetShaderInfoLog(vertex_shader, sizeof(error), &error_length, error);

		log_error_exit("Failed to compile vertex shader.\n%.*s", error_length, error);
	}

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shdr_compile_status);
	if (shdr_compile_status == GL_FALSE)
	{
		char error[4096];
		GLsizei error_length;

		glGetShaderInfoLog(fragment_shader, sizeof(error), &error_length, error);

		log_error_exit("Failed to compile fragment shader.\n%.*s", error_length, error);
	}

	shader->handle = glCreateProgram();

	glAttachShader(shader->handle, vertex_shader);
	glAttachShader(shader->handle, fragment_shader);

	glLinkProgram(shader->handle);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	free(vertex_shader_buffer);
	free(fragment_shader_buffer);
}
