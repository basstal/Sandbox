#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;

layout(binding = 0) uniform sampler2D texture0;

// grayscale
void main()
{
    FragColor     = texture(texture0, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor     = vec4(average, average, average, 1.0f);
}
