#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_texCoord;

void main()
{
	v_texCoord = texCoord;
	gl_Position = position;

}