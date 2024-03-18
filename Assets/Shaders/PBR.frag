#version 450
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoords;
layout(location = 1) in vec3 WorldPos;
layout(location = 2) in vec3 Normal;
layout(binding = 2) uniform sampler2D textures[4];// 0: albedoMap, 1: metallicMap, 2: roughnessMap, 3: aoMap TODO: 用不同序号的 binding

layout(binding = 3) uniform Light {
    vec3 position;
    vec3 color;
}
light;

//layout(binding = 3) uniform samplerCube irradianceMap;

layout(push_constant) uniform PushConstants {
    vec3 camPos;
}
pushConstants;


const float PI = 3.14159265359;

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main() {
    vec3 albedo     = pow(texture(textures[0], TexCoords).rgb, vec3(2.2));
    float metallic  = texture(textures[1], TexCoords).r;
    float roughness = texture(textures[2], TexCoords).r;
    float ao        = texture(textures[3], TexCoords).r;

    vec3 N = normalize(Normal);
    vec3 V = normalize(pushConstants.camPos - WorldPos);

    vec3 Lo = vec3(0.0);
    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo, metallic);

    //        for (int i = 0; i < 4; ++i)
    {
        // calculate per-light radiance
        vec3  L           = normalize(light.position - WorldPos);
        vec3  H           = normalize(V + L);
        float dist        = length(light.position - WorldPos);
        float attenuation = 1.0 / (dist * dist);
        vec3  radiance    = light.color * attenuation;


        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3  numerator   = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3  specular    = numerator / max(denominator, 0.001);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
//    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = vec3(1.0) * albedo;
    vec3 ambient = (kD * diffuse) * ao;

    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
