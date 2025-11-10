#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 rgb;
out vec4 vertex_color;

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    // vertex_color = vec4(pos.x + 0.5, 0.0, (pos.x + pos.y + 1.0) / 2.0, 1.0);
    vertex_color = vec4(rgb, 1.0);
}