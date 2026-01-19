#pragma once
#include <glm/ext/scalar_uint_sized.hpp>

namespace Charcoal::Color {
/**
 * @brief Helper function to pack 8-bit RGB components into a 
 * single 32-bit unsigned int according to SDL_PIXELFORMAT_RGBA32
 *
 * @param r Clamped to [0, 255]
 * @param g Clamped to [0, 255]
 * @param b Clamped to [0, 255]
 * @param a Clamped to [0, 255]
 */
glm::uint32 pack_rgba32(int r, int g, int b, int a);

/**
 * @brief Helper function to pack 8-bit RGB components into a
 * single 32-bit unsigned int according to SDL_PIXELFORMAT_RGBA32
 *
 * @param r Clamped to [0.0f, 1.0f]
 * @param g Clamped to [0.0f, 1.0f]
 * @param b Clamped to [0.0f, 1.0f]
 * @param a Clamped to [0.0f, 1.0f]
 */
glm::uint32 pack_rgba32(float r, float g, float b, float a);
}