#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Charcoal {
/**
 * @class Vertex
 * @brief Defines a Vertex, the base data structure to be submitted to shaders.
 *
 */
struct Vertex {
    glm::vec3 position;
    glm::uint32 rgb;
    glm::vec2 uv;

    Vertex();
    Vertex(const glm::vec3 &position);
    Vertex(const glm::vec3 &position, const glm::vec3 &rgb);
    Vertex(const glm::vec3 &position, glm::uint32 rgb);
    Vertex(const glm::vec3 &position, const glm::vec3 &rgb,
            const glm::vec2 &uv);
    Vertex(const glm::vec3 &position, glm::uint32 rgb, const glm::vec2 &uv);
};

} // namespace Charcoal
