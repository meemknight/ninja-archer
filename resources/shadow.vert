#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 u_shadow_lightSpaceMatrix;
uniform mat4 u_shadow_model;

void main()
{
    gl_Position = u_shadow_lightSpaceMatrix * u_shadow_model * vec4(aPos, 1.0);
}  