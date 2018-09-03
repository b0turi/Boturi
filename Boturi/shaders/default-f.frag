#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 2) in vec3 lightPosition;
layout(location = 3) in vec3 lightColor;

layout(location = 4) in vec3 objNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 unitNormal = normalize(objNormal);
    vec3 unitLightVector = normalize(lightPosition);

    float nDot1 = dot(unitNormal, unitLightVector);
    float brightness = max(nDot1, 0.0);
    vec3 diffuse = brightness * lightColor;
    vec3 ambient = 0.3 * lightColor;

    outColor = vec4(ambient, 1.0) + vec4(diffuse, 1.0);
}
