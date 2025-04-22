uniform float shiness;

// 5 Diffuse
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal){

	float diffuse = clamp(dot(-lightDir, normal), 0.0f, 1.0f);
	vec3 diffuseColor = lightColor * diffuse * objectColor;

	return diffuseColor;
}


// 6 Specular
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity){

	// 6.1 Remove the light from the back
	float dotResult = dot(-lightDir, normal);
	float flag = step(0.0, dotResult);

	// 6.2 Calculate reflection
	vec3 lightReflect = normalize(reflect(lightDir, normal));
	float specular = max(dot(lightReflect,-viewDir), 0.0);

	// 6.3 Control the size
	specular = pow(specular, shiness);

//	float specularMask = texture(specularMaskSampler, uv).r;

	// 6.4 Calculate specular color
	vec3 specularColor = lightColor * specular * flag * intensity;

	return specularColor;
}


// 7 Spot light
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir){

	// 7.1 Prepare variables
	vec3 objectColor = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(worldPosition - light.position);
	vec3 targetDir = normalize(light.targetDirection);

	float cGamma = dot(lightDir, targetDir);
	float intensity = clamp((cGamma - light.outerLine) / (light.innerLine - light.outerLine), 0.0, 1.0);

	// 7.2 Diffuse reflection
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);

	// 7.3 Specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);

	// 7.4 Firal result
	return (diffuseColor + specularColor) * intensity;
}


// 8 Directional light
vec3 calculateDirectionalLight(vec3 objectColor, DirectionalLight light, vec3 normal, vec3 viewDir){
	
	light.color *= light.intensity;

	// 7.1 Prepare variables
	vec3 lightDir = normalize(light.direction);


	// 7.2 Diffuse reflection
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);

	// 7.3 Specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);

	// 7.4 Firal result
	return diffuseColor + specularColor;

}

// 9 Point light
vec3 calculatePointLight(vec3 objectColor, PointLight light, vec3 normal, vec3 viewDir){
	
	vec3 lightDir = normalize(worldPosition - light.position);

	float dist = length(worldPosition - light.position);
	float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);

	// 9.2 Diffuse reflection
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);

	// 9.3 Specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);

	return (diffuseColor + specularColor) * attenuation;

}
