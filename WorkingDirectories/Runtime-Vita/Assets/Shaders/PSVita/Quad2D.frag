varying in float4 inColor : TEXCOORD0;
varying in float2 inTexCoord : TEXCOORD1;
varying in float inTexIndex : TEXCOORD2;
varying in float inTilingFactor : TEXCOORD3;

uniform sampler2D BlankTexture : TEXUNIT1;

float4 main() : COLOR {
    float4 textureColor = inColor * tex2D(BlankTexture, inTexCoord * inTilingFactor);

    // TEMPORARY, using the value as otherwise it'll be ignored and cause offsets.
    if (inTexIndex > 653.0)
        discard;

    if (textureColor.a <= 0.0)
        discard;

    return textureColor;
}