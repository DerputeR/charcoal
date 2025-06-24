#include "engine.h"
#include <string>
#include "shader_loader.h"
#include <SDL3/SDL_log.h>

Engine::Engine() {
    glGenBuffers(1, &this->vbo);
}

Engine::~Engine() {
    glDeleteBuffers(1, &this->vbo);
}

void Engine::add_triangles(const std::vector<Vertex>& vertices) {
    size_t size = vertices.size();
    if (size % 3 != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to add triangles; buffer contains %zu vertices", size);
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
}

void Engine::load_default_shaders() {
    ShaderLoader::create_program(ShaderLoader::DEFAULT_VERT_SRC, ShaderLoader::DEFAULT_FRAG_SRC);

}