#version 450

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in flat float inTexIndex;
layout(location = 3) in float inTilingFactor;

layout(binding = 1) uniform sampler2D textures[16];

layout(location = 0) out vec4 outColor;

void main() {
    vec4 textureColor = inColor;

    switch(int(inTexIndex))
    {
        case  0: textureColor *= texture(textures[ 0], inTexCoord * inTilingFactor); break;
        case  1: textureColor *= texture(textures[ 1], inTexCoord * inTilingFactor); break;
        case  2: textureColor *= texture(textures[ 2], inTexCoord * inTilingFactor); break;
        case  3: textureColor *= texture(textures[ 3], inTexCoord * inTilingFactor); break;
        case  4: textureColor *= texture(textures[ 4], inTexCoord * inTilingFactor); break;
        case  5: textureColor *= texture(textures[ 5], inTexCoord * inTilingFactor); break;
        case  6: textureColor *= texture(textures[ 6], inTexCoord * inTilingFactor); break;
        case  7: textureColor *= texture(textures[ 7], inTexCoord * inTilingFactor); break;
        case  8: textureColor *= texture(textures[ 8], inTexCoord * inTilingFactor); break;
        case  9: textureColor *= texture(textures[ 9], inTexCoord * inTilingFactor); break;
        case 10: textureColor *= texture(textures[10], inTexCoord * inTilingFactor); break;
        case 11: textureColor *= texture(textures[11], inTexCoord * inTilingFactor); break;
        case 12: textureColor *= texture(textures[12], inTexCoord * inTilingFactor); break;
        case 13: textureColor *= texture(textures[13], inTexCoord * inTilingFactor); break;
        case 14: textureColor *= texture(textures[14], inTexCoord * inTilingFactor); break;
        case 15: textureColor *= texture(textures[15], inTexCoord * inTilingFactor); break;
    }

    if (textureColor.a <= 0.0)
        discard;

    outColor = textureColor;
}