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

    /**
     * @brief Helper class to set vertex color using integer RGB representation
     * @param r Clamped to [0, 255]
     * @param g Clamped to [0, 255]
     * @param b Clamped to [0, 255]
     */
    void set_rgb(int r, int g, int b);

    /**
     * @brief Helper class to pack 8-bit RGB components into a single 32-bit
     * unsigned int
     *
     * @param r Clamped to [0, 255]
     * @param g Clamped to [0, 255]
     * @param b Clamped to [0, 255]
     */
    static glm::uint32 pack_rgb24_to_uint32(int r, int g, int b);

    static glm::uint32 pack_normalized_rgb24_to_uint32(
            float r, float g, float b);
    static glm::uint32 pack_normalized_rgb24_to_uint32(const glm::vec3 rgb);
};

} // namespace Charcoal
