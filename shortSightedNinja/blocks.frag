#version 330
layout (location = 0) out vec4 color;

uniform sampler2D u_sprites;

in vec2 v_texCoord;

void main()
{
	color = vec4(texture(u_sprites,v_texCoord).xyz,1);

}