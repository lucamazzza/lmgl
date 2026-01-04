#version 410 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_Color;
in vec2 v_TexCoord;

out vec4 FragColor;

void main() {
    FragColor = v_Color;
}
