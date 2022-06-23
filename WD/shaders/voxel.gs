#version 460 core

const float _VoxelSize = 0.5f;

layout(points) in;
layout(triangle_strip, max_vertices=12) out;

flat in struct {
	vec3 position;
	vec3 color;
	int neighbours;
} vs_out[];

uniform mat4 _Transform;
uniform mat4 _ViewProjection;
uniform vec3 _CameraLookDirection;

out struct {
	vec3 normal; // TEMP: cheap lighting
	vec3 color;
} gs_out;

int CameraCheck(const vec3 normal)
{
	float facing = dot(normal, normalize(_CameraLookDirection));
	return facing < 0.0f ? 1 : 0;
}

int NeighbourCheck(const int neighbours, const int neighbourIndex)
{
	return (neighbours & neighbourIndex) == neighbourIndex ? 1 : 0;
}

void main()
{
	gs_out.color = vs_out[0].color;

	int neighbours = vs_out[0].neighbours;
	vec3 centre = vec3(_Transform * vec4(vs_out[0].position, 1.0f));

	// top face
	if (CameraCheck(vec3(0.0f, 1.0f, 0.0f)) == 1 && NeighbourCheck(neighbours, 0x08) == 0)
	{
		gs_out.normal = vec3(0.0f, 1.0f, 0.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, _VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, _VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
	
	// bottom face
	if (CameraCheck(vec3(0.0f, -1.0f, 0.0f)) == 1 && NeighbourCheck(neighbours, 0x04) == 0)
	{
		gs_out.normal = vec3(0.0f, -1.0f, 0.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, -_VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, -_VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
	
	// front face
	if (CameraCheck(vec3(0.0f, 0.0f, -1.0f)) == 1 && NeighbourCheck(neighbours, 0x10) == 0)
	{
		gs_out.normal = vec3(0.0f, 0.0f, -1.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize,  _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize,  _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
	
	// back face
	if (CameraCheck(vec3(0.0f, 0.0f, 1.0f)) == 1 && NeighbourCheck(neighbours, 0x20) == 0)
	{
		gs_out.normal = vec3(0.0f, 0.0f, 1.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize,  _VoxelSize, _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize,  _VoxelSize, _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, -_VoxelSize, _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3( _VoxelSize, -_VoxelSize, _VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
	
	// left face
	if (CameraCheck(vec3(-1.0f, 0.0f, 0.0f)) == 1 && NeighbourCheck(neighbours, 0x01) == 0)
	{
		gs_out.normal = vec3(-1.0f, 0.0f, 0.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize,  -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize,   _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize, -_VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(-_VoxelSize,  _VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
	
	// right face
	if (CameraCheck(vec3(1.0f, 0.0f, 0.0f)) == 1 && NeighbourCheck(neighbours, 0x02) == 0)
	{
		gs_out.normal = vec3(1.0f, 0.0f, 0.0f);
		gl_Position = _ViewProjection * vec4(centre + vec3(_VoxelSize,  -_VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(_VoxelSize,   _VoxelSize, -_VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(_VoxelSize, -_VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		gl_Position = _ViewProjection * vec4(centre + vec3(_VoxelSize,  _VoxelSize,  _VoxelSize), 1.0f);
		EmitVertex();
	
		EndPrimitive();
	}
}
