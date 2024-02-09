#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

struct PointLight{
	vec4 position; // ignore w 
	vec4 color; // w - intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;
	vec4 ambientLightColor; // w - intensity
	PointLight pointLights[10];
	int numLights;
	bool fogEnabled;
	vec2 movingLightIndices;
	vec3 movingLightDirection;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

const float fogIntensity = 0.9;
const vec3 fogColor = vec3(0.5, 0.5, 0.5);

float CalcFog(float fogInt, vec3 cameraPos, vec3 fragPos){
	if (fogInt == 0) return 1.0;
	float gradient = (fogInt * fogInt - 50 * fogInt + 60);
	float distance = length(cameraPos - fragPos);
	float fog = exp(-pow((distance / gradient), 4));
	return clamp(fog, 0.0, 1.0);
}


void main(){
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPosWorld;

		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);
		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		float spotlightEffect = 1.0;
		if ((i == ubo.movingLightIndices[1] || i == ubo.movingLightIndices[0])) {
			// Calculate spotlight effect
			vec3 movingDirection = ubo.movingLightDirection;
			if(i == ubo.movingLightDirection[1]) movingDirection.x *= -1;
			vec3 spotlightDirection = normalize(movingDirection);
			float cosSpotlightAngle = dot(-directionToLight, spotlightDirection);
			spotlightEffect = smoothstep(0.9, 1.0, cosSpotlightAngle); 
		}

		diffuseLight += intensity * cosAngIncidence * spotlightEffect;

		// specular lighting Phong-Blinn
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 1024.0); // higher values -> sharper highlights
		specularLight += intensity * blinnTerm * spotlightEffect;
	}

	vec4 finalColor = vec4((diffuseLight * fragColor + specularLight * fragColor), 1.0);

	if(ubo.fogEnabled) {
		float fogFactor = CalcFog(fogIntensity, cameraPosWorld, fragPosWorld);
		finalColor = vec4(mix(fogColor, finalColor.xyz, fogFactor), 1.0);
	}

	outColor = finalColor;
	
}