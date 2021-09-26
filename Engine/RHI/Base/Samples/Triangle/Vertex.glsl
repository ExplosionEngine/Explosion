#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 0) out vec3 outColor;

layout (binding = 0) uniform Ubo {
    float aspect;
    float angle;
} ubo;

void main()
{
    vec2 outPos;
    outPos.x = (position.x * cos(ubo.angle) - position.y * sin(ubo.angle));
    outPos.y = (position.x * sin(ubo.angle) + position.y * cos(ubo.angle)) * ubo.aspect;
    gl_Position = vec4(outPos, 0.0, 1.0);
    outColor = color;
}