#version 410 core
    
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in vec3 a_Tangent;
layout(location = 5) in vec3 a_Bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_MVP;
uniform mat3 u_NormalMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec4 v_Color;
out vec2 v_TexCoord;
out mat3 v_TBN;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = u_NormalMatrix * a_Normal;
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    vec3 T = normalize(u_NormalMatrix * a_Tangent);
    vec3 B = normalize(u_NormalMatrix * a_Bitangent);
    vec3 N = normalize(v_Normal);
    v_TBN = mat3(T, B, N);
    gl_Position = u_MVP * vec4(a_Position, 1.0);
}
