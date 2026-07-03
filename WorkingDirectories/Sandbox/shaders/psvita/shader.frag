varying in float2 inTexCoord : TEXCOORD0;

uniform sampler2D TestTexture : TEXUNIT1;
uniform sampler2D TextureNiva : TEXUNIT2;

float4 main() : COLOR {
    float4 colorA = tex2D(TestTexture, inTexCoord);
    float4 colorB = tex2D(TextureNiva, inTexCoord);
    float dist = length(inTexCoord - float2(0.5, 0.5));

    float4 finalColor = (dist > 0.5) ? colorB : colorA;
    return finalColor;
}