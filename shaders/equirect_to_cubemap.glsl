#shader vertex
#version 410 core
layout (location = 0) in vec3 a_Position;

out vec3 v_LocalPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main() {
    v_LocalPos = a_Position;
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}

#shader fragment
#version 410 core
out vec4 FragColor;

in vec3 v_LocalPos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sample_spherical_map(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sample_spherical_map(normalize(v_LocalPos));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    FragColor = vec4(color, 1.0);
}
