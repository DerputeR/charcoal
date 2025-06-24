#pragma once
#include <glad/glad.h>
#include <vector>

/**
 * @brief Data struct for vertices
 */
struct Vertex {
    float x;
    float y;
    float z;
};

class Engine {
    GLuint vbo;
public:
    Engine();
    ~Engine();

    /**
     * @brief Adds triplets of vertices as triangles.
     * 
     * If the given vector's size is not divisible by 3,
     * this function will fail fast, and an error will be logged to SDL's logger.
     * @param vertices A vector of Vertex objects
     */
    void add_triangles(const std::vector<Vertex>& vertices);

    void load_default_shaders();
};