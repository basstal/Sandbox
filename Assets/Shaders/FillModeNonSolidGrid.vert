#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 fragColor;

layout (binding = 0) uniform ViewAndProjection {
    mat4 view;
    mat4 proj;
}
ubo;

layout (binding = 1) uniform Model {
    mat4 model;
} uboInstance;

void main() {
    gl_Position = ubo.proj * ubo.view * uboInstance.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
