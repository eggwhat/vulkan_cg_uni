#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec2 fragUV;

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


float CalcFog(float fogInt, vec3 cameraPos, vec3 fragPos){
	if (fogInt == 0) return 1.0;
	float gradient = (fogInt * fogInt - 50 * fogInt + 60);
	float distance = length(cameraPos - fragPos);
	float fog = exp(-pow((distance / gradient), 4));
	return clamp(fog, 0.0, 1.0);
}

const float fogIntensity = 0.9;
const vec3 fogColor = vec3(1.0, 1.0, 1.0);

void main(){
	vec3 final = fragColor;

	if(ubo.fogEnabled) {
		vec4 camPosWorld = ubo.invView[3];
		float fogFactor = CalcFog(fogIntensity, camPosWorld.xyz, fragPositionWorld);
		final = mix(fogColor, final, fogFactor);
	}

    //final = pow(final, vec3(1.0 / 2.2));
    outColor = vec4(final, 1.0);
}