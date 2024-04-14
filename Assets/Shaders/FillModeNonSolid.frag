#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 Normal;

layout (location = 0) out vec4 outColor;

layout (binding = 2) uniform FixedColor {
    vec3 c;
} fixedColor;

void main() {
    outColor = vec4(fixedColor.c, 1.0f);// texture(texSampler, fragTexCoord);
}
