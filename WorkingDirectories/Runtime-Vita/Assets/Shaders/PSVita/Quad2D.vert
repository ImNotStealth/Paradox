varying out float4 gl_Position : POSITION;

uniform Camera {
    float4x4 viewProj;
} Camera : BUFFER[0];

varying in float3 inPosition;
varying in float4 inColor;
varying in float2 inTexCoord;
varying in float inTexIndex;
varying in float inTilingFactor;

varying out float4 outColor : TEXCOORD0;
varying out float2 outTexCoord : TEXCOORD1;
varying out float outTexIndex : TEXCOORD2;
varying out float outTilingFactor : TEXCOORD3;

void main() {
    gl_Position = mul(float4(inPosition, 1.0), Camera.viewProj);
    outColor = inColor;
    outTexCoord = inTexCoord;
    outTexIndex = inTexIndex;
    outTilingFactor = inTilingFactor;
}