varying out float4 gl_Position : POSITION;

varying in float2 inPosition;
varying in float2 inTexCoord;

varying out float2 outTexCoord : TEXCOORD0;

void main() {
    gl_Position = float4(inPosition, 0.0, 1.0);
    outTexCoord = inTexCoord;
}