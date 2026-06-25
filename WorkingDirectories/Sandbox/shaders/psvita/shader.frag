varying in float4 outColor : TEXCOORD0;
varying in float2 inTexCoord : TEXCOORD1;
uniform sampler2D TestTexture : TEXUNIT2;
uniform sampler2D TextureNiva : TEXUNIT3;

float4 main() : COLOR {
    float4 colorA = tex2D(TestTexture, inTexCoord);
    float4 colorB = tex2D(TextureNiva, inTexCoord);
    float distance = length(inTexCoord - float2(0.5, 0.5));
    return (distance > 0.5) ? colorB : colorA;
}