#version 330 core
in vec4 vertex_color;
in vec2 vertex_uv;

uniform sampler2D obj_texture;
uniform sampler2D glass_texture;
uniform float blend;

out vec4 FragColor;

void main() {
    // FragColor = vertex_color;
    vec4 base = texture(obj_texture, vertex_uv);
    vec4 overlay = texture(glass_texture, vertex_uv);
    FragColor = mix(base, overlay, overlay.a * blend);
}
