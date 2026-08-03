#version 450

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(binding = 1) uniform sampler2D testTexture;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 textureColor = texture(testTexture, inTexCoord);

    if (textureColor.a <= 0.0)
        discard;

    outColor = textureColor * inColor;
}