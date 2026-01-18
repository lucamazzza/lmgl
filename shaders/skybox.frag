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
