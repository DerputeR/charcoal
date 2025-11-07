#version 330 core
in vec4 vertex_color;
out vec4 FragColor;

void main() {
    // FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    FragColor = vertex_color;
}