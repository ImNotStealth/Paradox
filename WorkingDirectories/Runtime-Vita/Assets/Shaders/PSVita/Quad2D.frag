varying in float4 inColor : TEXCOORD0;
varying in float2 inTexCoord : TEXCOORD1;

uniform sampler2D TestTexture : TEXUNIT1;

float4 main() : COLOR {
    float4 textureColor = tex2D(TestTexture, inTexCoord);

    if (textureColor.a <= 0.0)
        discard;

    return textureColor * inColor;
}