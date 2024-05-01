#version 450
layout (location = 0) in vec3 aPos;

layout (location = 0)out vec3 TexCoords;

layout (binding = 0) uniform ViewAndProjection {
    mat4 view;
    mat4 projection;
}
ubo;

void main()
{
    vec4 pos = ubo.projection * ubo.view * vec4(aPos, 1.0);
    TexCoords = aPos;
    gl_Position = pos.xyww;
}