#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
	
layout(location = 0) out vec4 fragColor;

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
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;


void main() {
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projection * ubo.view * positionWorld;

	vec3 finalColor = vec3(0.0, 0.0, 0.0);
	for(int i = 0; i < ubo.numLights; i++){
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - positionWorld;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
		directionToLight = normalize(directionToLight);
		float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;

		// specular lighting Phong-Blinn
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlights
		specularLight += intensity * blinnTerm;
	}
	fragColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}
