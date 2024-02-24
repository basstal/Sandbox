#version 450
layout (location = 0) in vec3 aPos;

layout (location = 0) out vec3 localPos;

layout(binding = 0) uniform MVPObject {
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;

void main()
{
    localPos = aPos;
    gl_Position = ubo.proj * ubo.view * vec4(aPos, 1.0);
}