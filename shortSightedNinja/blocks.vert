#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 mainColor;

out vec2 v_texCoord;
out vec4 v_mainColor;

void main()
{
	v_mainColor = mainColor;
	v_texCoord = texCoord;
	gl_Position = position;
}