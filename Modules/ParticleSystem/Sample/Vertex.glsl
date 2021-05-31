#version 450

layout (location = 0) in vec3 position;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    gl_Position = ubo.proj * ubo.view * vec4(position, 1.0);
    gl_Position.y = -gl_Position.y;
    gl_PointSize = 4.f;
}