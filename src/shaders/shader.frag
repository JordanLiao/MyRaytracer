#version 330 core

//uniform vec3 diffuseColor;
uniform float threshold;

in float dist;
in vec3 norm;
out vec4 fragColor;

void main() {
  float absDist = abs(dist);
  float alpha = 1.0f - absDist / threshold;
  if(alpha > 0.f)                                                                                                                                                         
    fragColor = vec4(norm, alpha);
  else 
    fragColor = vec4(0.0f,0.0f,0.0f,0.0f);
}