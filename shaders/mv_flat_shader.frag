#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec3 fragPosWorld;
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
} ubo;

layout(set = 0, binding = 2) uniform sampler2D image;

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
	vec3 finalColor = vec3(0.0);
	vec3 imageColor = texture(image, fragUV).xyz;
	finalColor += fragColor * ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

    for (int i = 0; i < ubo.numLights; ++i) {
		PointLight light = ubo.pointLights[i];
        vec3 lightDirection = normalize(light.position.xyz - fragPosWorld);
		if((i == ubo.movingLightIndices [1] || i == ubo.movingLightIndices[0]) && lightDirection.y > 0.0) {
			continue;
		}
        float intensity = max(dot(normalize(fragNormalWorld), lightDirection), 0.0);
        finalColor += intensity * light.color.xyz * light.color.w * fragColor;
    }
	finalColor *= imageColor;

	if(ubo.fogEnabled) {
		vec3 cameraPosWorld = ubo.invView[3].xyz;
		float fogFactor = CalcFog(fogIntensity, cameraPosWorld, fragPosWorld);
		finalColor = mix(fogColor, finalColor, fogFactor);
	}

    outColor = vec4(finalColor, 1.0);
	
}