#shader vertex
#version 410 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_tex_coord;

out vec2 v_tex_coord;

uniform mat4 u_projection;

void main() {
    v_tex_coord = a_tex_coord;
    gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
}

#shader fragment
#version 410 core
in vec2 v_tex_coord;
out vec4 frag_color;

uniform sampler2D u_text;
uniform vec4 u_color;

void main() {
    float alpha = texture(u_text, v_tex_coord).r;
    frag_color = vec4(u_color.rgb, u_color.a * alpha);
}
