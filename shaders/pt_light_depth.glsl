#shader vertex
#version 410 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_Model;

void main() {
    gl_Position = u_Model * vec4(a_Position, 1.0);
}

#shader geometry
#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_ShadowMatrices[6];

out vec4 v_FragPos;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {
            v_FragPos = gl_in[i].gl_Position;
            gl_Position = u_ShadowMatrices[face] * v_FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
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
