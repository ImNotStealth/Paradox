varying in float2 inTexCoord : TEXCOORD0;

float4 main() : COLOR {
    return float4(inTexCoord.rg, 0.0, 1.0);
}