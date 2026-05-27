#version 450

layout(binding = 0) uniform Camera {
    mat4 viewProj;
} camera;

layout(binding = 1) uniform Color {
    vec3 color;
} color;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = camera.viewProj * vec4(inPosition, 0.0, 1.0);
    outColor = color.color;
}