#include "volume.h"

#include "utility.h"

void volume_init(struct volume* volume)
{
	volume->length_outer_x = 64;
	volume->length_outer_y = 64;
	volume->length_outer_z = 64;
	volume->volume_outer = volume->length_outer_x * volume->length_outer_y * volume->length_outer_z;

	volume->length_inner_x = volume->length_outer_x - 2;
	volume->length_inner_y = volume->length_outer_y - 2;
	volume->length_inner_z = volume->length_outer_z - 2;
	volume->volume_inner = volume->length_inner_x * volume->length_inner_y * volume->length_inner_z;

	transform_init(&volume->transform);

	volume->blocks = calloc(volume->volume_outer, 1);
	if (volume->blocks == NULL)
	{
		allocation_failure();
	}

	volume->colors = malloc(volume->volume_inner * 3);
	if (volume->colors == NULL)
	{
		allocation_failure();
	}

	darray_init(&volume->vertices, sizeof(struct vertex), 10000);

	volume->vao = 0;
	volume->vbo = 0;
	volume->meshed = false;
	volume->element_count = 0;
}

void volume_free(struct volume* volume)
{
	darray_free(&volume->vertices);

	free(volume->blocks);
	free(volume->colors);
}

size_t get_index(int x, int y, int z, struct volume* volume)
{
	return y * volume->length_inner_x * volume->length_inner_z + z * volume->length_inner_x + x;
}

size_t get_expanded_index(size_t index, struct volume* volume)
{
	size_t y = (index / (volume->length_inner_x * volume->length_inner_z));
	size_t z = ((index % (volume->length_inner_x * volume->length_inner_z)) / volume->length_inner_x);
	size_t x = (index % volume->length_inner_x);

	x++;
	y++;
	z++;

	return y * volume->length_outer_x * volume->length_outer_z + z * volume->length_outer_x + x;
}

size_t get_expanded_index2(struct volume* volume, int x, int y, int z)
{
	return y * volume->length_outer_x * volume->length_outer_z + z * volume->length_outer_x + x;
}

size_t get_index_relative_to2(size_t index, struct volume* volume, int x, int y, int z)
{
	return index + y * volume->length_inner_z * volume->length_inner_x + z * volume->length_inner_x + x;
}

size_t get_index_relative_to(size_t index, size_t length_x, size_t length_z, int x, int y, int z)
{
	return index + y * length_x * length_z + z * length_x + x;
}

static int calculate_vertex_ao(bool side1, bool side2, bool corner)
{
	if (side1 && side2)
	{
		return 0;
	}

	return 3 - (side1 + side2 + corner);
}

void volume_mesh(struct volume* volume)
{
	darray_empty(&volume->vertices);

	bool* blocks = volume->blocks;
	GLubyte* colors = volume->colors;

	GLubyte vx;
	GLubyte vy;
	GLubyte vz;

	GLubyte vr;
	GLubyte vg;
	GLubyte vb;

	bool ao_neighbors[8];
	int a00;
	int a10;
	int a11;
	int a01;

	int barycentric0 = 0 << 3;
	int barycentric1 = 1 << 3;
	int barycentric2 = 2 << 3;

	struct vertex vertex;

	size_t expanded;

	for (size_t i = 0; i < volume->volume_inner; i++)
	{
		expanded = get_expanded_index(i, volume);

		if (blocks[expanded])
		{
			// Shared
			vy = i / (volume->length_inner_x * volume->length_inner_z);
			vz = (i % (volume->length_inner_x * volume->length_inner_z)) / volume->length_inner_x;
			vx = i % volume->length_inner_x;

			vr = colors[i * 3 + 0];
			vg = colors[i * 3 + 1];
			vb = colors[i * 3 + 2];

			// -Y
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, -1, 0)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, 1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, -1, 1)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, 1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, 0)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, -1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, -1, -1)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, -1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, 0)];

				a00 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a10 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);
				a11 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);
				a01 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 5 / 01
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a01 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a10 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 4 / 11
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a11 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 5 / 01
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a10 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 4 / 11
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a11 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 5 / 01
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a00 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 4 / 11
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a11 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 0 | a10 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}

			// -X
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 0, 0)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, -1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, 0)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, 1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 0, 1)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, 1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, 0)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, -1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 0, -1)];

				a00 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);
				a10 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);
				a11 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a01 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 5 / 10
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 7 / 11
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a11 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 5 / 10
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a10 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 5 / 10
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a10 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 7 / 11
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 7 / 11
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a11 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 1 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}

			// -Z
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 0, -1)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, -1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, -1, -1)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, -1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 0, -1)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, -1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 1, -1)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, -1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 0, -1)];

				a00 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);
				a01 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a11 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);
				a10 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 2 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 2 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 1 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 0 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 3 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 2 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 2 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}

			// +Z
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 0, 1)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, -1, 1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, -1, 1)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, 1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 0, 1)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, 1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 1, 1)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, 1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 0, 1)];

				a00 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);
				a10 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);
				a11 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a01 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a11 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 5 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a10 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 5 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a10 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 5 / 00
					vertex.x = vx;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a11 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 3 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}

			// +X
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 0, 0)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, 1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, 0)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, -1, -1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 0, -1)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, -1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, 0)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, 1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 0, 1)];

				a00 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);
				a01 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a11 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);
				a10 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 0 / 00
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 2 / 01
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 0 / 00
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 0 / 00
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 2 / 01
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 4 / 10
					vertex.x = vx + 1;
					vertex.y = vy;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 2 / 01
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 4 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}

			// +Y
			if (!blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 1, 0)])
			{
				ao_neighbors[0] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, -1)];
				ao_neighbors[1] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 1, -1)];
				ao_neighbors[2] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, -1)];
				ao_neighbors[3] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, 0)];
				ao_neighbors[4] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, -1, 1, 1)];
				ao_neighbors[5] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 0, 1, 1)];
				ao_neighbors[6] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, 1)];
				ao_neighbors[7] = blocks[get_index_relative_to(expanded, volume->length_outer_x, volume->length_outer_z, 1, 1, 0)];

				a00 = calculate_vertex_ao(ao_neighbors[1], ao_neighbors[3], ao_neighbors[2]);
				a01 = calculate_vertex_ao(ao_neighbors[3], ao_neighbors[5], ao_neighbors[4]);
				a11 = calculate_vertex_ao(ao_neighbors[5], ao_neighbors[7], ao_neighbors[6]);
				a10 = calculate_vertex_ao(ao_neighbors[7], ao_neighbors[1], ao_neighbors[0]);

				a00 <<= 5;
				a10 <<= 5;
				a11 <<= 5;
				a01 <<= 5;

				if ((a00 + a11) > (a01 + a10))
				{
					// 3 / 00
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a00 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 2 / 10
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 3 / 00
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
				else
				{
					// 2 / 10
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 3 / 00
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a00 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a01 | barycentric2;
					darray_append(&volume->vertices, &vertex);

					// 2 / 10
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a10 | barycentric0;
					darray_append(&volume->vertices, &vertex);

					// 7 / 01
					vertex.x = vx;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a01 | barycentric1;
					darray_append(&volume->vertices, &vertex);

					// 6 / 11
					vertex.x = vx + 1;
					vertex.y = vy + 1;
					vertex.z = vz + 1;
					vertex.r = vr;
					vertex.g = vg;
					vertex.b = vb;
					vertex.bits = 5 | a11 | barycentric2;
					darray_append(&volume->vertices, &vertex);
				}
			}
		}
	}

	volume->meshed = true;
}

void volume_upload(struct volume* volume)
{
	glDeleteVertexArrays(1, &volume->vao);
	glDeleteBuffers(1, &volume->vbo);

	glGenVertexArrays(1, &volume->vao);
	glBindVertexArray(volume->vao);

	glGenBuffers(1, &volume->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, volume->vbo);
	glBufferData(GL_ARRAY_BUFFER, volume->vertices.length * volume->vertices.element_size, volume->vertices.buffer, GL_STREAM_DRAW);

	glVertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, 7, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 7, (GLvoid*)3);
	glEnableVertexAttribArray(1);

	glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, 7, (GLvoid*)6);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	volume->element_count = volume->vertices.length;
}

void volume_export(struct volume* volume)
{
	int low_x = volume->length_outer_x - 1;
	int low_y = volume->length_outer_y - 1;
	int low_z = volume->length_outer_z - 1;

	for (int y = 2; y < volume->length_outer_y; y++)
	{
		for (int z = 0; z < volume->length_outer_z; z++)
		{
			for (int x = 0; x < volume->length_outer_x; x++)
			{
				if (volume->blocks[get_expanded_index2(volume, x, y, z)])
				{
					low_x = min(low_x, x);
					low_y = min(low_y, y);
					low_z = min(low_z, z);
				}
			}
		}
	}

	low_x--;
	low_y--;
	low_z--;

	for (int i = 0; i < volume->vertices.length; i++)
	{
		struct vertex* vertex = darray_get(&volume->vertices, i);

		vertex->x -= low_x;
		vertex->y -= low_y;
		vertex->z -= low_z;
	}

	FILE* file = fopen("export.bin", "wb");

	if (file)
	{
		fwrite(volume->vertices.buffer, 1, volume->vertices.element_size * volume->vertices.length, file);
	}

	fclose(file);
}
