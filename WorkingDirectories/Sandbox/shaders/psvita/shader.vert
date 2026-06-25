varying out float4 gl_Position : POSITION;

uniform Camera {
    float4x4 viewProj;
} Camera : BUFFER[0];

uniform Color {
    float4 color;
} Color : BUFFER[1];

varying in float3 inPosition;
varying in float3 inColor;
varying in float2 inTexCoord;

varying out float4 outColor : TEXCOORD0;
varying out float2 outTexCoord : TEXCOORD1;

void main() {
    gl_Position = mul(float4(inPosition, 1.0), Camera.viewProj);
    outColor = Color.color * float4(inColor, 1.0);
    outTexCoord = inTexCoord;
}