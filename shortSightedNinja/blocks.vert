#version 330
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 mainColor;
layout(location = 3) in vec4 sideColor;
layout(location = 4) in vec2 colorTextCoord;

out vec2 v_texCoord;
out vec2 v_texColorCoord;
out vec4 v_mainColor;
out vec4 v_sideColor;

void main()
{
	v_texColorCoord = colorTextCoord;
	v_sideColor = sideColor;
	v_mainColor = mainColor;
	v_texCoord = texCoord;
	gl_Position = position;
}