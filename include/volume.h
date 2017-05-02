#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <stdbool.h>

#include <GL/glew.h>

#include "darray.h"
#include "transform.h"
#include "vertex.h"

struct volume
{
	size_t length_outer_x;
	size_t length_outer_y;
	size_t length_outer_z;
	size_t volume_outer;

	size_t length_inner_x;
	size_t length_inner_y;
	size_t length_inner_z;
	size_t volume_inner;

	struct transform transform;

	bool* blocks;
	GLubyte* colors;

	GLuint vao;
	GLuint vbo;

	bool meshed;
	struct darray vertices;
	GLsizei element_count;
};

void volume_init(struct volume* volume);

void volume_free(struct volume* volume);

void volume_mesh(struct volume* volume);

void volume_upload(struct volume* volume);

void volume_export(struct volume* volume);

size_t get_index(int x, int y, int z, struct volume* volume);

size_t get_expanded_index(size_t index, struct volume* volume);

size_t get_expanded_index2(struct volume* volume, int x, int y, int z);

size_t get_index_relative_to(size_t index, size_t length_x, size_t length_z, int x, int y, int z);

size_t get_index_relative_to2(size_t index, struct volume* volume, int x, int y, int z);

#endif
