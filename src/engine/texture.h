#pragma once
#include <SDL3/SDL_surface.h>
#include <glad/glad.h>

namespace Charcoal {
class Texture {
    SDL_Surface *surface;
    static SDL_Surface *init_missing_texture();

public:
    explicit Texture();
    explicit Texture(SDL_Surface *surface);
    
    // copy constructor - make sure to copy the underlying data, too
    Texture(const Texture &other);
    Texture &operator=(const Texture &other);

    ~Texture() noexcept;

    void *get_pixels() const;
    int get_width() const;
    int get_height() const;
};

class TextureLoader {
public:
    static Texture load_from_png(const char *filepath);
};

class GpuTexture {
    GLuint id;

public:
    explicit GpuTexture();

    // move constructors
    GpuTexture(GpuTexture &&other) noexcept;
    GpuTexture &operator=(GpuTexture &&other) noexcept;

    // don't allow copying
    GpuTexture(const GpuTexture &other) = delete;
    GpuTexture &operator=(const GpuTexture &other) = delete;

    ~GpuTexture() noexcept;
    
    void upload(const Texture &texture);
    void bind();

    bool is_valid() const;
};
}