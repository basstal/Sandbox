#version 450
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec3 Normal;

layout (binding = 0) uniform ViewAndProjection {
    mat4 view;
    mat4 projection;
}
ubo;

layout (binding = 1) uniform Model {
    mat4 model;
} uboInstance;

void main() {
    gl_Position = ubo.projection * ubo.view * uboInstance.model * vec4(inPosition, 1.0);
    WorldPos = vec3(uboInstance.model * vec4(inPosition, 1.0));
    Normal = mat3(transpose(inverse(uboInstance.model))) * inNormal;
    TexCoords = inTexCoords;
}
