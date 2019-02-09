#version 130

in vec4 a_position;
in vec2 a_coord;

out vec2 uv;

void main() {
  uv = a_coord;
  gl_Position = a_position;
}


