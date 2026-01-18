#shader vertex
#version 410 core
layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
    v_TexCoords = a_Position;
    vec4 pos = u_Projection * u_View * vec4(a_Position, 1.0);
    gl_Position = pos.xyww; // Trick to always render at max depth
}

#shader fragment
#version 410 core
out vec4 FragColor;

in vec3 v_TexCoords;

uniform samplerCube u_Skybox;
uniform float u_Exposure;

void main() {
    vec3 color = texture(u_Skybox, v_TexCoords).rgb;
    color = vec3(1.0) - exp(-color * u_Exposure);
    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
}
