#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <stdexcept>

namespace chip8::graphics::sdl_wrappers
{
    /// <summary>
    /// SDL3 Window deleter smart pointer adapter
    /// </summary>
    struct SDLWindowDeleter final
    {
        void operator()(SDL_Window* window) const noexcept
        {
            SDL_DestroyWindow(window);
        }
    };

    /// <summary>
    /// SDL3 Renderer deleter smart pointer adapter
    /// </summary>
    struct SDLRendererDeleter final
    {
        void operator()(SDL_Renderer* renderer) const noexcept
        {
            SDL_DestroyRenderer(renderer);
        }
    };

    /// <summary>
    /// SDL3 Rexture deleter smart pointer adapter
    /// </summary>
    struct SDLTextureDeleter final
    {
        void operator()(SDL_Texture* texture) const noexcept
        {
            SDL_DestroyTexture(texture);
        }
    };

    // SDL3 unique pointer to a Window instance.
    using WindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

    // SDL3 unique pointer to a Renderer instance.
    using RendererPtr = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>;

    // SDL3 unique pointer to a Texture instance.
    using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

    /// <summary>
    /// RAII SDL Context class.
    /// Copy and move operations are disabled.
    /// </summary>
    class SDLContext final
    {
    public:
        explicit SDLContext(SDL_InitFlags flags)
        {
            if (!SDL_Init(flags)) {
                throw std::runtime_error(SDL_GetError());
            }
        }

        ~SDLContext() { SDL_Quit(); }

        // disable copy operations
        SDLContext(const SDLContext&) = delete;
        SDLContext& operator=(const SDLContext&) = delete;

        // disable move operations
        SDLContext(SDLContext&&) = delete;
        SDLContext& operator=(SDLContext&&) = delete;
    };

} // chip8::graphics::sdl_wrappers
