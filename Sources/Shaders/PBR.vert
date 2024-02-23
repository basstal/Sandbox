#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 TexCoords;
layout(location = 1) out vec3 WorldPos;
layout(location = 2) out vec3 Normal;

layout(binding = 0) uniform MVPObject {
    mat4 model;
    mat4 view;
    mat4 proj;
}
ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    WorldPos    = vec3(ubo.model * vec4(inPosition, 1.0));
    Normal      = mat3(transpose(inverse(ubo.model))) * inNormal;
    TexCoords   = inTexCoords;
}