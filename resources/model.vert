#version 330 core

layout (location = 0) in vec3 a_positions;
layout (location = 1) in vec3 a_normals;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_ModelViewProjection;
uniform mat4 u_modelToWorld;

out vec3 v_worldSpacePos;
out vec3 v_normals;
out vec2 v_uv;

void main()
{
	gl_Position = u_ModelViewProjection * vec4(a_positions, 1);

	v_worldSpacePos = vec3(u_modelToWorld * vec4(a_positions, 1)).xyz;
	
	v_normals = (u_modelToWorld * vec4(a_normals,0)).xyz; //uniform scale
	//v_normals = mat3(transpose(inverse(mat3(u_modelToWorld)))) * a_normals;  //non uniform scale

	v_normals = normalize(v_normals);

	v_uv = a_uv;
}