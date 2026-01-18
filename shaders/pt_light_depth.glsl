#shader vertex
#version 410 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_Model;

void main() {
    gl_Position = u_Model * vec4(a_Position, 1.0);
}

#shader fragment
#version 410 core
in vec4 v_FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() {
    float distance = length(v_FragPos.xyz - u_LightPos);
    distance = distance / u_FarPlane;
    gl_FragDepth = distance;
}
