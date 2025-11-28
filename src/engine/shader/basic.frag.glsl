#version 330 core
in vec4 vertex_color;
in vec2 vertex_uv;

uniform sampler2D obj_texture;

out vec4 FragColor;

void main() {
    // FragColor = vertex_color;
    FragColor = texture(obj_texture, vertex_uv);
}
