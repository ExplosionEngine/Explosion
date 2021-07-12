#version 450

layout (location = 0) in vec3 position;

#include <Common/DefaultObj.esli>
#include <Common/DefaultView.esli>

StorageBuffer(GLOBAL, OutBuffer, 0) {
    float value[];
};

void main()
{
    gl_Position = vec4(position, 1.0);
}