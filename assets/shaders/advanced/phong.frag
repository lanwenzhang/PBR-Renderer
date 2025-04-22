#version 460 core


out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform float opacity;
uniform float intensity;
uniform vec3 ambientColor;
uniform float shiness;
uniform vec3 cameraPosition;

uniform sampler2D sampler;
uniform sampler2D specularMaskSampler;

#include "../common/lightStruct.glsl"

uniform DirectionalLight directionalLight;

void main()
{
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 result = vec3(0.0, 0.0, 0.0);
	
	// 1 Prepare common variable
	vec3 normalN = normalize(normal);
	vec3 viewDir = normalize(worldPosition - cameraPosition);


	// 2 Calculate directional light diffuse and specular
	result += calculateDirectionalLight(objectColor, directionalLight, normalN, viewDir);
	

	// 3 Ambient reflection
	float alpha = texture(sampler, uv).a;


	vec3 ambientColor = objectColor * ambientColor;

	vec3 finalColor = result + ambientColor;


	FragColor = vec4(finalColor, alpha * opacity);
}