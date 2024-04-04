#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out vec3 Normal;

layout (binding = 0) uniform MVPObject {
    mat4 view;
    mat4 proj;
}
ubo;

layout (binding = 1) uniform Model {
    mat4 model;
} uboInstance;

void main() {
    gl_Position = ubo.proj * ubo.view * uboInstance.model * vec4(inPosition, 1.0);
    //    gl_Position = vec4(inPosition, 1.0);
    fragColor = vec3(1.0f);// white
    Normal = inNormal;
    fragTexCoord = inTexCoords;
}
