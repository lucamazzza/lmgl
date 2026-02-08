#shader vertex
#version 410 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_transform;
uniform mat4 u_projection;

void main() {
    v_tex_coord = a_tex_coord;
    gl_Position = u_projection * u_transform * vec4(a_position, 0.0, 1.0);
}

#shader fragment
#version 410 core
in vec2 v_tex_coord;
out vec4 frag_color;

uniform sampler2D u_texture;
uniform vec4 u_tint;

void main() {
    frag_color = texture(u_texture, v_tex_coord) * u_tint;
}
