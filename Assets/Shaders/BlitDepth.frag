#version 450
layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec2 fragUv;

layout(input_attachment_index = 0, set = 0, binding = 0) uniform subpassInputMS depthInputMS;


void main()
{
    int   samples = 8;  // 根据实际的多采样数修改
    float depth   = 0.0;
    for (int i = 0; i < samples; ++i)
    {
        vec4 depthSample = subpassLoad(depthInputMS, i);
        depth += depthSample.r;  // 累加所有样本的深度值
    }
    depth /= samples;  // 计算平均深度
    fragColor = vec4(depth, depth, depth, 1.0);
}
