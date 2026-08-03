#version 450

layout(location = 0) in vec2 inTexCoord;

layout(binding = 1) uniform sampler2D textureArrayTest[2];

layout(location = 0) out vec4 outColor;

void main() {
    vec4 colorA = texture(textureArrayTest[0], inTexCoord);
    vec4 colorB = texture(textureArrayTest[1], inTexCoord);
    float distance = length(inTexCoord - vec2(0.5, 0.5));
    vec4 finalColor = (distance > 0.5) ? colorB : colorA;

    if (finalColor.a < 0.25)
        discard;

    outColor = finalColor;
}