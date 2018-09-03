#version 450
#extension GL_ARB_separate_shader_objects : enable

const int NUM_LIGHTS = 2;

layout(std140, set = 0, binding = 1) uniform MVPMatrix {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(std140, set = 0, binding = 2) uniform Light {
    vec4 position[NUM_LIGHTS];
    vec4 color[NUM_LIGHTS];
} l;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(location = 2) out vec3 lightPosition[NUM_LIGHTS];
layout(location = (2 + NUM_LIGHTS)) out vec3 lightColor[NUM_LIGHTS];

layout(location = (2 + NUM_LIGHTS * 2)) out vec3 objNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;

    fragColor = inColor;
    fragTexCoord = inTexCoord;

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
      lightPosition[i] = l.position[i].xyz - worldPosition.xyz;
      lightColor[i] = l.color[i].xyz;
    }

    objNormal = (ubo.model * vec4(inNormal, 0.0)).xyz;
}
