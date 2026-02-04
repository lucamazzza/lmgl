#shader vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in vec3 a_Tangent;
layout(location = 5) in vec3 a_Bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_MVP;
uniform mat3 u_NormalMatrix;
uniform mat4 u_LightSpaceMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_Color;
out vec2 v_TexCoord;
out mat3 v_TBN;
out vec4 v_FragPosLightSpace;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = u_NormalMatrix * a_Normal;
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    vec3 T = normalize(u_NormalMatrix * a_Tangent);
    vec3 B = normalize(u_NormalMatrix * a_Bitangent);
    vec3 N = normalize(v_Normal);
    v_TBN = mat3(T, B, N);
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}

#shader fragment
#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_Color;
in vec2 v_TexCoord;
in mat3 v_TBN;
in vec4 v_FragPosLightSpace;

out vec4 FragColor;

// Material properties
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 emissive;

    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
    sampler2D emissiveMap;

    int hasAlbedoMap;
    int hasNormalMap;
    int hasMetallicMap;
    int hasRoughnessMap;
    int hasAoMap;
    int hasEmissiveMap;
};

// Light structure
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
};

uniform Material u_Material;
uniform vec3 u_CameraPos;

// Lights (simple for now)
uniform int u_NumDirLights;
uniform DirectionalLight u_DirLights[4];

uniform int u_NumPointLights;
uniform PointLight u_PointLights[16];

uniform sampler2D u_ShadowMap;
uniform int u_UseShadows;

const float PI = 3.14159265359;

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    if (u_UseShadows == 0) return 0.0;
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // Reduced bias to prevent holes in shadows
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) shadow = 0.0;
    return shadow;
}

void main() {
    // Sample material properties
    vec3 albedo = u_Material.albedo;
    if (u_Material.hasAlbedoMap == 1) {
        albedo = texture(u_Material.albedoMap, v_TexCoord).rgb;
    }

    float metallic = u_Material.metallic;
    if (u_Material.hasMetallicMap == 1) {
        metallic = texture(u_Material.metallicMap, v_TexCoord).r;
    }

    float roughness = u_Material.roughness;
    if (u_Material.hasRoughnessMap == 1) {
        roughness = texture(u_Material.roughnessMap, v_TexCoord).r;
    }

    float ao = u_Material.ao;
    if (u_Material.hasAoMap == 1) {
        ao = texture(u_Material.aoMap, v_TexCoord).r;
    }

    vec3 emissive = u_Material.emissive;
    if (u_Material.hasEmissiveMap == 1) {
        emissive = texture(u_Material.emissiveMap, v_TexCoord).rgb;
    }

    // Get normal
    vec3 N;
    if (u_Material.hasNormalMap == 1) {
        // Sample normal from normal map
        vec3 normalMap = texture(u_Material.normalMap, v_TexCoord).rgb;
        // Transform from [0,1] to [-1,1]
        normalMap = normalMap * 2.0 - 1.0;
        // Transform to world space using TBN matrix
        N = normalize(v_TBN * normalMap);
    } else {
        N = normalize(v_Normal);
    }

    vec3 V = normalize(u_CameraPos - v_FragPos);

    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // Reflectance equation
    vec3 Lo = vec3(0.0);

    // Directional lights
    for (int i = 0; i < u_NumDirLights; ++i) {
        vec3 L = normalize(-u_DirLights[i].direction);
        vec3 H = normalize(V + L);
        vec3 radiance = u_DirLights[i].color * u_DirLights[i].intensity;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        float shadow = ShadowCalculation(v_FragPosLightSpace, N, L);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

    // Point lights
    for (int i = 0; i < u_NumPointLights; ++i) {
        vec3 L = normalize(u_PointLights[i].position - v_FragPos);
        vec3 H = normalize(V + L);
        float distance = length(u_PointLights[i].position - v_FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_PointLights[i].color * u_PointLights[i].intensity * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient lighting (simplified)
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo + emissive;

    // Output raw HDR color (post-process will handle tone mapping and gamma)
    FragColor = vec4(color, 1.0);
}
