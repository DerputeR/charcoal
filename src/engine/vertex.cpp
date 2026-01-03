#include "vertex.h"
#include "glm/ext/scalar_common.hpp"
#include "color.h"
#include <algorithm>

namespace Charcoal {
Vertex::Vertex() : position{0.0f, 0.0f, 0.0f}, color{0xFFFFFF}, uv{0.0f, 0.0f} {
}

Vertex::Vertex(const glm::vec3 &position) :
        position{position}, color{0x00000000}, uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, const glm::vec3 &color) :
        position{position}, color{Color::pack_rgba32(color.r, color.g, color.b, 1.0f)},
        uv{position.x, position.y} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 color) :
        position{position}, color{color}, uv{position.x, position.y} {
}

Vertex::Vertex(
        const glm::vec3 &position, const glm::vec3 &color, const glm::vec2 &uv) :
        position{position}, color{Color::pack_rgba32(color.r, color.g, color.b, 1.0f)}, uv{uv} {
}

Vertex::Vertex(const glm::vec3 &position, glm::uint32 color,
        const glm::vec2 &uv) : position{position}, color{color}, uv{uv} {
}
} // namespace Charcoal
