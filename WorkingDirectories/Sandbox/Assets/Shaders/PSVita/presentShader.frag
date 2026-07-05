varying in float2 inTexCoord : TEXCOORD0;

uniform sampler2D PresentTexture : TEXUNIT0;

float4 main() : COLOR {
    return tex2D(PresentTexture, inTexCoord);
}