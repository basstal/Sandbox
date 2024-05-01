#version 450
layout(location = 0)out vec4 FragColor;

layout(location = 0)in vec3 TexCoords;

layout(binding = 1)uniform samplerCube cubeTexture;
void main()
{
    FragColor = texture(cubeTexture, TexCoords);//vec4(TexCoords, 1.0f);
}