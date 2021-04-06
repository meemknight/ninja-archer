#version 330 core

in vec3 v_worldSpacePos;
in vec3 v_normals;
in vec2 v_uv;

out vec4 color;

vec3 lightPos = normalize(vec3(-0.5, 2.5, 1.2));

vec3 ambientLight = vec3(1, 1, 1);
vec3 diffuseLight = vec3(1, 1, 1);
vec3 specularLight = vec3(0.5, 0.5, 0.5);
float specularExponent = 32;

uniform vec3 u_eyePosition;
uniform sampler2D u_albedo;

void main()
{

	vec3 normal = normalize(v_normals);
	vec3 lightDir = normalize(lightPos - v_worldSpacePos);
	
	vec3 albedo = texture2D(u_albedo, v_uv).rgb;

	float kd = dot(v_normals, lightDir);
	float ka = 0.3;

		
	vec3 viewDir    = normalize(u_eyePosition - v_worldSpacePos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float ks = pow(max(dot(normal, halfwayDir), 0.0), specularExponent);


	color.a = 1;
	color.rgb = ambientLight * ka + diffuseLight * kd + specularLight * ks;
	color.rgb = clamp(color.rgb, 0, 1);
	
	color.rgb *= albedo;


}