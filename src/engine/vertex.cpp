#include "vertex.h"
#include "glm/ext/scalar_common.hpp"
#include <algorithm>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f}, rgb{0xFFFFFF}, uv{0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) :
        position{position}, rgb{0xFFFFFF}, uv{position.x, position.y} {
}

// TODO: clamp the rgb values
Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &rgb) :
        position{position}, rgb{pack_normalized_rgb24_to_uint32(rgb)},
        uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb) :
        position{position}, rgb{rgb}, uv{position.x, position.y} {
}

Vertex::Vertex(
        const glm::vec3 &position, const glm::vec3 &rgb, const glm::vec2 &uv) :
        position{position}, rgb{pack_normalized_rgb24_to_uint32(rgb)}, uv{uv} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb,
        const glm::vec2 &uv) : position{position}, rgb{rgb}, uv{uv} {
}

void Vertex::set_rgb(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    rgb = (r << 16) | (g << 8) | b;
}

glm::uint32 Vertex::pack_rgb24_to_uint32(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    glm::uint32 rgb = (r << 16) | (g << 8) | b;
    return rgb;
}

glm::uint32 Vertex::pack_normalized_rgb24_to_uint32(float r, float g, float b) {
    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);
    int ri = glm::uround(r * 255.0f);
    int gi = glm::uround(g * 255.0f);
    int bi = glm::uround(b * 255.0f);
    return pack_rgb24_to_uint32(ri, gi, bi);
}

glm::uint32 Vertex::pack_normalized_rgb24_to_uint32(const glm::vec3 rgb) {
    return pack_normalized_rgb24_to_uint32(rgb.r, rgb.g, rgb.b);
}

} // namespace Charcoal
