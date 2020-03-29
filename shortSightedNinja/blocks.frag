#version 330
layout (location = 0) out vec4 color;

uniform sampler2D u_sprites;

in vec2 v_texCoord;
in vec4 v_mainColor;

void main()
{
	color = v_mainColor * vec4(texture(u_sprites,v_texCoord).xyz,1);

}