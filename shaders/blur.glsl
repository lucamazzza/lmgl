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

uniform sampler2D u_Texture;
uniform int u_Horizontal;
uniform float u_Weights[5];

void main() {
    vec2 tex_offset = 1.0 / textureSize(u_Texture, 0);
    vec3 result = texture(u_Texture, v_TexCoord).rgb * u_Weights[0];
    if (u_Horizontal == 1) {
        for(int i = 1; i < 5; ++i) {
            result += texture(u_Texture, v_TexCoord + vec2(tex_offset.x * i, 0.0)).rgb * u_Weights[i];
                 result += texture(u_Texture, v_TexCoord - vec2(tex_offset.x * i, 0.0)).rgb * u_Weights[i];
             }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(u_Texture, v_TexCoord + vec2(0.0, tex_offset.y * i)).rgb * u_Weights[i];
            result += texture(u_Texture, v_TexCoord - vec2(0.0, tex_offset.y * i)).rgb * u_Weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
