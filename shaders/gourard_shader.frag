#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;

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
	vec3 albedo = texture(image, fragUV).xyz;
	vec3 final = albedo * fragColor;
    final = pow(final, vec3(1.0 / 2.2));
    outColor = vec4(final, 1.0);
}