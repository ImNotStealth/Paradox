#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D texSamplerNiva;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 colorA = texture(texSampler, inTexCoord);
    vec4 colorB = texture(texSamplerNiva, inTexCoord);
    float distance = length(inTexCoord - vec2(0.5, 0.5));
    outColor = (distance > 0.5) ? colorB : colorA;
}
