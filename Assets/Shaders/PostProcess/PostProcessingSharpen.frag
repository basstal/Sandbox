#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D texture0;

float offset = 1.0f / 300.0f;

void main()
{
    // Sharpen
    float kernal[9] = float[](0, -1, 0, -1, 5, -1, 0, -1, 0);

    vec2 offsets[9] = vec2[](vec2(-offset, offset), vec2(0, offset), vec2(offset, offset), vec2(-offset, 0), vec2(0, 0), vec2(offset, 0), vec2(-offset, -offset),
                             vec2(0, -offset), vec2(offset, -offset));
    vec3 sampleTex[9];
    for (int i = 0; i < 9; ++i)
    {
        sampleTex[i] = vec3(texture(texture0, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0f);
    for (int i = 0; i < 9; ++i)
    {
        col += kernal[i] * sampleTex[i];
    }

    FragColor = vec4(col, 1.0f);
}
