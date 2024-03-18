#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;

layout(binding = 1) uniform sampler2D inputTexture;

void main() {
    FragColor = texture(inputTexture, TexCoords);
}