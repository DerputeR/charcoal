#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in uint rgb;
layout (location = 2) in vec2 uv;
uniform mat4 transform;
out vec4 vertex_color;
out vec2 vertex_uv;

void main() {
    gl_Position = transform * vec4(pos, 1.0f);
    vertex_color = vec4(
        ((rgb & 0xFF0000u) >> 16) / 255.0,
        ((rgb & 0x00FF00u) >> 8) / 255.0,
        ((rgb & 0x0000FFu) >> 0) / 255.0,
        1.0
    );
    vertex_uv = uv;
    // vertex_color = vec4(1.0, 0.0, 1.0, 1.0);
}
