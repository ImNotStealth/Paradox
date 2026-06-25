#version 450

layout(binding = 0) uniform Camera {
    mat4 viewProj;
} camera;

layout(binding = 1) uniform Color {
    vec3 color;
} color;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main() {
    gl_Position = camera.viewProj * vec4(inPosition, 1.0);
    outColor = color.color;
    outTexCoord = inTexCoord;
}
