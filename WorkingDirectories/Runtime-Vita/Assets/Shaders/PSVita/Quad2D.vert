varying out float4 gl_Position : POSITION;

uniform Camera {
    float4x4 viewProj;
} Camera : BUFFER[0];

varying in float3 inPosition;
varying in float4 inColor;

varying out float4 outColor : TEXCOORD0;

void main() {
    gl_Position = mul(float4(inPosition, 1.0), Camera.viewProj);
    outColor = inColor;
}