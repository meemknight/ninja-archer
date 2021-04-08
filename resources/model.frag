#version 330 core

in vec3 v_worldSpacePos;
in vec3 v_normals;
in vec2 v_uv;

out vec4 a_outColor;

uniform vec3 u_lightPos[10];
uniform int u_lightCount;
uniform float u_color;

vec3 ambientLight = vec3(1, 1, 1);
vec3 diffuseLight = vec3(1, 1, 1);
vec3 specularLight = vec3(0.5, 0.5, 0.5);
float specularExponent = 32;

uniform vec3 u_eyePosition;
uniform sampler2D u_albedo;


const float PI = 3.14159265359;

//n normal
//h halfway vector
//a roughness	(1 rough, 0 glossy) 
//this gets the amount of specular light reflected
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness*roughness;
	float a2     = a*a;
	float NdotH  = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return  a2 / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num   = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}
 
//oclude light that is hidded begind small geometry roughnesses
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2  = GeometrySchlickGGX(NdotV, roughness);
	float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

//cosTheta is the dot between the normal and halfway
//ratio between specular and diffuse reflection
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 computePointLightSource(vec3 lightPosition, float metallic, float roughness, in vec3 lightColor, in vec3 worldPosition,
	in vec3 viewDir, in vec3 color, in vec3 normal)
{

	vec3 lightDirection = normalize(lightPosition - worldPosition);
	vec3 halfwayVec = normalize(lightDirection + viewDir);
	
	float dist = length(lightPosition - worldPosition);
	float attenuation = 1.0 / pow(dist,2);
	attenuation = 1; //(option) remove light attenuation
	vec3 radiance = lightColor * attenuation; //here the first component is the light color
	
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, color.rgb, metallic);	//here color is albedo, metalic surfaces use albdeo
	vec3 F  = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.0), F0);

	float NDF = DistributionGGX(normal, halfwayVec, roughness);       
	float G   = GeometrySmith(normal, viewDir, lightDirection, roughness);   

	float denominator = 4.0 * max(dot(normal, viewDir), 0.0)  
		* max(dot(normal, lightDirection), 0.0);
	vec3 specular     = (NDF * G * F) / max(denominator, 0.001);

	vec3 kS = F; //this is the specular contribution
	vec3 kD = vec3(1.0) - kS; //the difuse is the remaining specular
	kD *= 1.0 - metallic;	//metallic surfaces are darker
	
	// scale light by NdotL
	float NdotL = max(dot(normal, lightDirection), 0.0);        
	vec3 Lo = (kD * color.rgb / PI + specular) * radiance * NdotL;

	return Lo;
}

void main()
{

	vec3 pos = v_worldSpacePos;
	vec3 normal = normalize(v_normals);
	vec3 albedo = texture2D(u_albedo, v_uv).rgb;
	albedo  = pow(albedo , vec3(2.2,2.2,2.2)).rgb; //gamma corection
	vec3 material = vec3(0.9, 0, 0.5); //roughness, metallic, ambient
	vec3 viewDir    = normalize(u_eyePosition - pos);

	vec3 Lo = vec3(0,0,0); //this is the accumulated light

	//foreach point light
	for(int i=0; i<u_lightCount;i++)
	{
		vec3 lightPosition = u_lightPos[i];
		vec3 lightColor = vec3(1,1,1);

		//vec3 phisicallyColor = computePointLightSource(lightPosition, material.g, material.r, lightColor, 
		//	pos, viewDir, albedo, normal);
		

		float val = dot(normalize(u_lightPos[i] - pos).xyz, normal.xyz);
		clamp(val, 0, 1);
	

		Lo += mix(vec3(0,0,0), vec3(1,1,1), vec3(val, val, val));


	}



	//vec3 ambientColor = vec3(0.03);
	//vec3 ambient = ambientColor * albedo.rgb * material.b; //this value is made up
	//vec3 color   = Lo + ambient; 
	
	//int steps = 25;
	//Lo *= steps;
	//Lo = floor(Lo);
	//Lo /= steps;

	vec3 color = albedo.rgb * Lo;

	//HDR 
	//color = color / (color + vec3(1.0));
	//float exposure = 1;
	//color = vec3(1.0) - exp(-color  * exposure);
	
	//todo move gamma 
	//gamma correction
	color = pow(color, vec3(1.0/2.2));


	a_outColor = clamp(vec4(color.rgb, 1), 0, 1);
	a_outColor *= u_color;
	
	


}