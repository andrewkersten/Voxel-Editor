#version 330

const vec3 ambient = vec3(1.0, 1.0, 1.0);
const float ambient_occlusion_strength = 0.6;

in vec3 Color;
in vec3 lighting;
in vec3 barycentric;
in float ambient_occlusion;

float edgeFactor()
{
    vec3 d = fwidth(barycentric);
    vec3 a3 = smoothstep(vec3(0.0), d * 1.5, barycentric);
    return min(min(a3.x, a3.y), a3.z);
}

void main()
{
	vec3 total_lighting = lighting + ambient * (ambient_occlusion * ambient_occlusion_strength + (1.0 - ambient_occlusion_strength));

	//gl_FragColor = vec4(Color * total_lighting, 1.0);

	gl_FragColor.rgb = mix(vec3(0.0), Color * total_lighting, edgeFactor());
}
