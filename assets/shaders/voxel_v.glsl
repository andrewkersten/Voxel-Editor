#version 330

#extension GL_ARB_explicit_uniform_location : enable

const vec3 normal_table[6] = vec3[6](
	vec3(0.0, -1.0, 0.0),
	vec3(-1.0, 0.0, 0.0),
	vec3(0.0, 0.0, -1.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0)
);

const vec3 barycentric_table[3] = vec3[3](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

const vec3 lightdir = vec3(0.0, -1.0, 0.0);
const vec3 lightcol = vec3(1.0, 1.0, 1.0);

layout (location = 0) uniform mat4 projection;
layout (location = 4) uniform mat4 modelview;
layout (location = 8) uniform mat3 normalmat;

layout (location = 0) in ivec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in int bits;

out vec3 Color;
out vec3 lighting;
out vec3 barycentric;
out float ambient_occlusion;

void main()
{
	Color = color;

	int barycentric_lookup = (bits >> 3) & 3;
	barycentric = barycentric_table[barycentric_lookup];

	ambient_occlusion = float(bits >> 5) / 3.0;

	int normal_lookup = bits & 7;
	vec3 normal = normal_table[normal_lookup];

	vec3 transformed_normal = normalmat * normal;
	float directional_weight = max(dot(transformed_normal, -normalize(lightdir)), 0.0);
	lighting = lightcol * directional_weight;

	gl_Position = projection * modelview * vec4(position, 1.0);
}
