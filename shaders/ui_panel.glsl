#shader vertex
#version 410 core
layout(location = 0) in vec2 a_position;

uniform mat4 u_transform;
uniform mat4 u_projection;

void main() {
    gl_Position = u_projection * u_transform * vec4(a_position, 0.0, 1.0);
}

#shader fragment
#version 410 core
out vec4 frag_color;

uniform vec4 u_color;

void main() {
    frag_color = u_color;
}
