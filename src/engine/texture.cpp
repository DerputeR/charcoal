#include "texture.h"
#include <SDL3/SDL_log.h>
#include "color.h"

namespace Charcoal {

Texture TextureLoader::load_from_png(const char *filepath) {
    // load the texture data
    SDL_Surface *initial_load = nullptr;
    initial_load = SDL_LoadPNG(filepath);
    if (initial_load != nullptr) {
        if (initial_load->format != SDL_PIXELFORMAT_RGBA32) {
            SDL_Surface *converted =
                    SDL_ConvertSurface(initial_load, SDL_PIXELFORMAT_RGBA32);
            SDL_DestroySurface(initial_load);
            return Texture(converted);
        } else {
            return Texture(initial_load);
        }
    } else {
        SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "Unable to load \"%s\": %s",
                filepath, SDL_GetError());
        return Texture();
    }
}

Texture::Texture() : surface{init_missing_texture()} {
    
}

Texture::Texture(SDL_Surface *surface) :
        surface{surface ? surface : init_missing_texture()} {
}

Texture::Texture(const Texture &other) {
    this->surface = SDL_DuplicateSurface(other.surface);
}

Texture &Texture::operator=(const Texture &other) {
    if (this != &other) {        
        this->surface = SDL_DuplicateSurface(other.surface);
    }
    return *this;
}

Texture::~Texture() {
    if (surface != nullptr) {
        SDL_DestroySurface(surface);
    }
}

SDL_Surface *Texture::init_missing_texture() {
    SDL_Surface *surface = SDL_CreateSurface(2, 2, SDL_PixelFormat::SDL_PIXELFORMAT_RGBA32);
    glm::uint32 *pixels = static_cast<glm::uint32 *>(surface->pixels);
    pixels[0] = Color::pack_rgba32(255, 0, 255, 255);
    pixels[1] = Color::pack_rgba32(0, 0, 0, 255);
    pixels[2] = Color::pack_rgba32(0, 0, 0, 255);
    pixels[3] = Color::pack_rgba32(255, 0, 255, 255);
    return surface;
}

void *Texture::get_pixels() const {
    if (surface != nullptr) {
        return surface->pixels;
    } else {
        return nullptr;
    }
}

int Texture::get_width() const {
    if (surface != nullptr) {
        return surface->w;
    } else {
        return 0;
    }
}

int Texture::get_height() const {
    if (surface != nullptr) {
        return surface->h;
    } else {
        return 0;
    }
}

GpuTexture::GpuTexture() : id{0} {
    glGenTextures(1, &id);
    // todo: set default parameters
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GpuTexture::GpuTexture(GpuTexture &&other) noexcept : id{other.id} {
    other.id = 0;
}

GpuTexture &GpuTexture::operator=(GpuTexture &&other) noexcept {
    if (this != &other) {
        this->id = other.id;
        other.id = 0;
    }
    return *this;
}

GpuTexture::~GpuTexture() noexcept {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
}

void GpuTexture::upload(const Texture &texture) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            texture.get_width(), texture.get_height(),
            0, GL_RGBA, GL_UNSIGNED_BYTE,
            texture.get_pixels());
    glGenerateMipmap(GL_TEXTURE_2D);
}

void GpuTexture::bind() {
    glBindTexture(GL_TEXTURE_2D, id);
}

bool GpuTexture::is_valid() const {
    return id != 0;
}

} // namespace Charcoal