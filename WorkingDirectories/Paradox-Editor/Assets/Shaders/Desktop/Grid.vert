#version 450

layout(binding = 0) uniform GridSettings {
    mat4 viewProj;
    vec4 color;
    float scale;
    float size;
} gridSettings;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main() {
    gl_Position = gridSettings.viewProj * vec4(inPosition, 1.0);
    outTexCoord = inTexCoord;
}
