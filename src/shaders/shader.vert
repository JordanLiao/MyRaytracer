#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in float distance;
layout (location = 2) in vec3 normal;

// Uniform variables
uniform mat4 projView;
uniform mat4 model;

out float dist;
out vec3 norm;

void main() {
  gl_Position = projView * model * vec4(position, 1.0);
  dist = distance;
  norm = normal;
}