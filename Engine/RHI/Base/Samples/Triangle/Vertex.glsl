#version 450

layout (location = 0) in vec3 position;

layout (binding = 0) uniform Ubo {
    float angle;
} ubo;

void main()
{
    vec2 outPos;
    outPos.x = position.x * cos(ubo.angle) - position.y * sin(ubo.angle);
    outPos.y = position.x * sin(ubo.angle) + position.y * cos(ubo.angle);
    gl_Position = vec4(outPos, 0.0, 1.0);
}