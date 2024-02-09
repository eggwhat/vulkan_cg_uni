#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
	
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec2 fragUV;

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

layout(push_constant) uniform Push{
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;


void main() {
    vec3 normals = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 ambientColour = vec3(0.0);
 
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);  
    vec3 camPosWorld = ubo.invView[3].xyz;
    normals = normalize(mat3(push.normalMatrix) * normal);
 
    vec3 camDir = normalize(camPosWorld - positionWorld.xyz);
 
    for (int i = 0; i < ubo.numLights; i++)
    {
        float radius = 5.0;
        PointLight light = ubo.pointLights[i];
        vec3 lightDir = normalize(light.position.xyz - positionWorld.xyz);
        vec3 halfAngle = normalize(camDir + lightDir);
        float NdotL = clamp(dot(normals, lightDir), 0.0, 1.0);
        float NdotH = clamp(dot(normals, halfAngle), 0.0, 1.0);
 
        float specularHighlight = pow(NdotH, 250);
        vec3 S = (light.color.xyz * light.color.w * specularHighlight);
        vec3 D = (light.color.xyz * light.color.w * NdotL);
        
        float dist = length(light.position.xyz - positionWorld.xyz);
        float attenuation = 1.0 / (1.0 + ((2.0 / radius) * dist) + ((1.0 / (radius * radius)) * (dist * dist)));
        
        diffuse += D * attenuation;
        specular += S * attenuation;
    }
    vec3 ambience = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    fragColor = ambience + diffuse + specular;
    fragPositionWorld = positionWorld.xyz;
    fragUV = uv;

	gl_Position = ubo.projection * ubo.view * push.modelMatrix * vec4(position, 1.0);
}