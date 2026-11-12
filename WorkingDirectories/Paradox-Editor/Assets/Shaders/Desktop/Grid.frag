#version 450

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform GridSettings {
    mat4 viewProj;
    vec4 color;
    float scale;
    float size;
} gridSettings;

float grid(vec2 st, float res)
{
    vec2 grid = fract(st);
    return step(res, grid.x) * step(res, grid.y);
}

void main() {
    float x = grid(inTexCoord * gridSettings.scale + (gridSettings.size / 2.0), gridSettings.size);
	outColor = gridSettings.color * (1.0 - x);
	
	if (outColor.a == 0.0)
		discard;
}