#version 450
layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D depthSampler;


void main()
{
    float depth   = texture(depthSampler, TexCoords).r;
    fragColor = vec4(depth, depth, depth, 1.0);
}
