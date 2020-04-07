#version 330
layout (location = 0) out vec4 color;

//uniform int u_time;

uniform sampler2D u_sprites;
uniform sampler2D u_up;
uniform sampler2D u_down;
uniform sampler2D u_left;
uniform sampler2D u_right;

in vec2 v_texCoord;
in vec4 v_mainColor;
in vec4 v_sideColor;
in vec2 v_texColorCoord;

vec3 rgbTohsv(vec3 c)
{
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
 

vec3 hsvTorgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{

	vec4 mainColor = clamp(v_mainColor, vec4(0,0,0,0), vec4(1,1,1,1));
	
	float lightBoost = texture2D(u_up,v_texColorCoord).x * v_sideColor.x * texture2D(u_up,v_texColorCoord).w;
	lightBoost = max(lightBoost, texture(u_down,v_texColorCoord).x * v_sideColor.y * texture2D(u_down,v_texColorCoord).w);
	lightBoost = max(lightBoost, texture(u_left,v_texColorCoord).x * v_sideColor.z * texture2D(u_left,v_texColorCoord).w);
	lightBoost = max(lightBoost, texture(u_right,v_texColorCoord).x * v_sideColor.w * texture2D(u_right,v_texColorCoord).w);

	lightBoost = min(lightBoost, 1);
	lightBoost *= 0.5;

	vec4 c =  texture(u_sprites,v_texCoord);
	color = mainColor  * c;

	color.rgb = rgbTohsv(color.rgb);

	color.b += (lightBoost);
	color.b = min(color.b, 1.f);

	//color.r += cos(u_time*0.0002)*0.02;
	//if(color.r>1){color.r-=1;}
	//if(color.r<0){color.r = 1+color.r;}

	color.rgb = hsvTorgb(color.rgb);

}