precision mediump float;

uniform mat4 viewProj;
uniform vec3 color;

attribute vec2 inPosition;
attribute vec3 inColor;

varying vec3 outColor;

void main() {
    gl_Position = viewProj * vec4(inPosition, 0.0, 1.0);
    outColor = color;
}