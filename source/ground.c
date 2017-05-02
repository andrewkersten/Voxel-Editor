#include "ground.h"

struct ground Ground;

void ground_init(void)
{
	transform_init(&Ground.transform);
	Ground.transform.translation = GLKVector3Make(1000.0f, 999.99f, 1000.0f);
	transform_calc(&Ground.transform);

	struct vertex_ground vertices[GROUND_VERTEX_COUNT];

	vertices[0].position.x = 64.0f;
	vertices[0].position.y = 0.0f;
	vertices[0].position.z = 0.0f;
	vertices[0].texcoord.u = 0.0f;
	vertices[0].texcoord.v = 0.0f;

	vertices[1].position.x = 64.0f;
	vertices[1].position.y = 0.0f;
	vertices[1].position.z = 64.0f;
	vertices[1].texcoord.u = 0.0f;
	vertices[1].texcoord.v = 64.0f;

	vertices[2].position.x = 0.0f;
	vertices[2].position.y = 0.0f;
	vertices[2].position.z = 64.0f;
	vertices[2].texcoord.u = 64.0f;
	vertices[2].texcoord.v = 64.0f;

	vertices[3].position.x = 64.0f;
	vertices[3].position.y = 0.0f;
	vertices[3].position.z = 0.0f;
	vertices[3].texcoord.u = 0.0f;
	vertices[3].texcoord.v = 0.0f;

	vertices[4].position.x = 0.0f;
	vertices[4].position.y = 0.0f;
	vertices[4].position.z = 64.0f;
	vertices[4].texcoord.u = 64.0f;
	vertices[4].texcoord.v = 64.0f;

	vertices[5].position.x = 0.0f;
	vertices[5].position.y = 0.0f;
	vertices[5].position.z = 0.0f;
	vertices[5].texcoord.u = 64.0f;
	vertices[5].texcoord.v = 0.0f;

	glGenVertexArrays(1, &Ground.vao);
	glBindVertexArray(Ground.vao);

	glGenBuffers(1, &Ground.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Ground.vbo);
	glBufferData(GL_ARRAY_BUFFER, GROUND_VERTEX_COUNT * sizeof(struct vertex_ground), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ground), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ground), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}
