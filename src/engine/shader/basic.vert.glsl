#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in uint rgb;
uniform float offset;
out vec4 vertex_color;

void main() {
    gl_Position = vec4(pos.x + offset, pos.y, pos.z, 1.0);
    vertex_color = vec4(
        ((rgb & 0xFF0000u) >> 16) / 255.0,
        ((rgb & 0x00FF00u) >> 8) / 255.0,
        ((rgb & 0x0000FFu) >> 0) / 255.0,
        1.0
    );
    // vertex_color = vec4(1.0, 0.0, 1.0, 1.0);
}
