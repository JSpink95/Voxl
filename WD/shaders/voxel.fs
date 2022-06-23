#version 460 core

in struct {
	vec3 normal; // TEMP: cheap lighting
	vec3 color;
} gs_out;

layout(location = 0) out vec4 o_color;

const vec3 _LightPosition = vec3(5.0f, 2.5f, 3.25f);
const vec3 _LightDirection = -normalize(_LightPosition);

void main()
{
	float nl = max(dot(gs_out.normal, -_LightDirection), 0.05f);
	vec3 color = gs_out.color;
	o_color = vec4(color * nl, 1.0f);
}
