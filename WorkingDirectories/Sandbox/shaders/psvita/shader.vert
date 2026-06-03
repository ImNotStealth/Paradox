varying out float4 gl_Position : POSITION;

uniform Camera {
    float4x4 viewProj;
} Camera : BUFFER[0];

uniform Color {
    float4 color;
} Color : BUFFER[1];

varying in float2 inPosition;
varying in float3 inColor;

varying out float4 outColor : TEXCOORD0;

void main() {
    gl_Position = mul(float4(inPosition, 0.0, 1.0), Camera.viewProj);
    outColor = Color.color;
}