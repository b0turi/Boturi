#version 450
#extension GL_ARB_separate_shader_objects : enable

const int NUM_LIGHTS = 2;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 2) in vec3 lightPosition[NUM_LIGHTS];
layout(location = (2 + NUM_LIGHTS)) in vec3 lightColor[NUM_LIGHTS];

layout(location = (2 + NUM_LIGHTS * 2)) in vec3 objNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 unitNormal = normalize(objNormal);

    vec3 totalLight = vec3(0.0);
    vec3 ambient = vec3(0.0);

    for(int i = 0; i < NUM_LIGHTS; i++)
    {
      vec3 unitLightVector = normalize(lightPosition[i]);
      float nDot1 = dot(unitNormal, unitLightVector);
      float brightness = max(nDot1, 0.0);
      totalLight = totalLight + brightness * lightColor[i];
      ambient = ambient + lightColor[i];
    }
    ambient = ambient * 0.3;

    outColor = (vec4(ambient, 1.0) + vec4(totalLight, 1.0)) * texture(texSampler, fragTexCoord);
}
