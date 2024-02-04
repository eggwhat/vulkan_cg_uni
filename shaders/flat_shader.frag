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
} ubo;

layout(set = 0, binding = 1) uniform sampler2D image;

layout(push_constant) uniform Push {
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

void main(){
	vec3 finalColor = vec3(0.0);
	vec3 imageColor = texture(image, fragUV).xyz;

    for (int i = 0; i < ubo.numLights; ++i) {
		PointLight light = ubo.pointLights[i];
        vec3 lightDirection = normalize(light.position.xyz - fragPosWorld);
        float intensity = max(dot(normalize(fragNormalWorld), lightDirection), 0.0);
        finalColor += intensity * light.color.xyz * light.color.w * fragColor;
    }
	finalColor *= imageColor;

    outColor = vec4(finalColor, 1.0);
	
}