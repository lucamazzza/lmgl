#shader vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 1.0);
}

#shader fragment
#version 410 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_ScreenTexture;
uniform sampler2D u_BloomTexture;
uniform int u_ToneMapMode; // 0 = none, 1 = reinhard, 2 = aces
uniform float u_Exposure;
uniform float u_Gamma;
uniform int u_BloomEnabled;
uniform float u_BloomIntensity;

vec3 reinhard(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 aces(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main() {
    vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;
    if (u_BloomEnabled == 1) {
        vec3 bloom = texture(u_BloomTexture, v_TexCoord).rgb;
        color += bloom * u_BloomIntensity;
    }
    color *= u_Exposure;
    if (u_ToneMapMode == 1) {
        color = reinhard(color);
    } else if (u_ToneMapMode == 2) {
        color = aces(color);
    }
    color = pow(color, vec3(1.0 / u_Gamma));
    FragColor = vec4(color, 1.0);
}
