varying in float4 inColor : TEXCOORD0;
varying in float2 inTexCoord : TEXCOORD1;
varying in float inTexIndex : TEXCOORD2;
varying in float inTilingFactor : TEXCOORD3;

uniform sampler2D Texture0 : TEXUNIT1;
uniform sampler2D Texture1 : TEXUNIT2;
uniform sampler2D Texture2 : TEXUNIT3;
uniform sampler2D Texture3 : TEXUNIT4;
uniform sampler2D Texture4 : TEXUNIT5;
uniform sampler2D Texture5 : TEXUNIT6;
uniform sampler2D Texture6 : TEXUNIT7;
uniform sampler2D Texture7 : TEXUNIT8;
uniform sampler2D Texture8 : TEXUNIT9;
uniform sampler2D Texture9 : TEXUNIT10;
uniform sampler2D Texture10 : TEXUNIT11;
uniform sampler2D Texture11 : TEXUNIT12;
uniform sampler2D Texture12 : TEXUNIT13;
uniform sampler2D Texture13 : TEXUNIT14;
uniform sampler2D Texture14 : TEXUNIT15;

float4 main() : COLOR {
    float4 textureColor = float4(1.0, 0.0, 1.0, 1.0);

    // Maybe find a better way to do this,
    short index = (short)inTexIndex;
    if (index == 0)         textureColor = tex2D(Texture0, inTexCoord * inTilingFactor);
    else if (index == 1)    textureColor = tex2D(Texture1, inTexCoord * inTilingFactor);
    else if (index == 2)    textureColor = tex2D(Texture2, inTexCoord * inTilingFactor);
    else if (index == 3)    textureColor = tex2D(Texture3, inTexCoord * inTilingFactor);
    else if (index == 4)    textureColor = tex2D(Texture4, inTexCoord * inTilingFactor);
    else if (index == 5)    textureColor = tex2D(Texture5, inTexCoord * inTilingFactor);
    else if (index == 6)    textureColor = tex2D(Texture6, inTexCoord * inTilingFactor);
    else if (index == 7)    textureColor = tex2D(Texture7, inTexCoord * inTilingFactor);
    else if (index == 8)    textureColor = tex2D(Texture8, inTexCoord * inTilingFactor);
    else if (index == 9)    textureColor = tex2D(Texture9, inTexCoord * inTilingFactor);
    else if (index == 10)    textureColor = tex2D(Texture10, inTexCoord * inTilingFactor);
    else if (index == 11)    textureColor = tex2D(Texture11, inTexCoord * inTilingFactor);
    else if (index == 12)    textureColor = tex2D(Texture12, inTexCoord * inTilingFactor);
    else if (index == 13)    textureColor = tex2D(Texture13, inTexCoord * inTilingFactor);
    else if (index == 14)    textureColor = tex2D(Texture14, inTexCoord * inTilingFactor);

    float4 finalColor = textureColor * inColor;
    if (finalColor.a <= 0.0)
        discard;

    return finalColor;
}