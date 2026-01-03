#include "vertex.h"
#include "glm/ext/scalar_common.hpp"
#include "color.h"
#include <algorithm>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f}, rgb{0xFFFFFF}, uv{0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) :
        position{position}, rgb{0x00000000}, uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &rgb) :
        position{position}, rgb{Color::pack_rgba32(rgb.r, rgb.g, rgb.b, 1.0f)},
        uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb) :
        position{position}, rgb{rgb}, uv{position.x, position.y} {
}

Vertex::Vertex(
        const glm::vec3 &position, const glm::vec3 &rgb, const glm::vec2 &uv) :
        position{position}, rgb{Color::pack_rgba32(rgb.r, rgb.g, rgb.b, 1.0f)}, uv{uv} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 rgb,
        const glm::vec2 &uv) : position{position}, rgb{rgb}, uv{uv} {
}
} // namespace Charcoal
