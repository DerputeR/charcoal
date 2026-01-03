#include "color.h"
#include <algorithm>
#include <glm/ext/scalar_common.hpp>
#include <glm/ext/scalar_uint_sized.hpp>

namespace Charcoal::Color {
glm::uint32 pack_rgba32(int r, int g, int b, int a) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    a = std::clamp(a, 0, 255);
    glm::uint32 rgb = (a << 24) | (b << 16) | (g << 8) | r;
    return rgb;
}

glm::uint32 pack_rgba32(float r, float g, float b, float a) {
    r = std::clamp(r, 0.0f, 1.0f);
    g = std::clamp(g, 0.0f, 1.0f);
    b = std::clamp(b, 0.0f, 1.0f);
    a = std::clamp(a, 0.0f, 1.0f);
    int ri = glm::uround(r * 255.0f);
    int gi = glm::uround(g * 255.0f);
    int bi = glm::uround(b * 255.0f);
    int ai = glm::uround(a * 255.0f);
    return pack_rgba32(ri, gi, bi, ai);
}
}