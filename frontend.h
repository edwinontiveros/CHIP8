#pragma once

#include "sdl_wrappers.h"
#include <SDL3/SDL.h>
#include <span>
#include <string_view>

namespace chip8::graphics::frontend
{
    class SDLFrontend final
    {
    public:
        SDLFrontend(int width, int height, std::string_view title);
        ~SDLFrontend() noexcept;

        // no copy operations
        SDLFrontend(const SDLFrontend&)             = delete;
        SDLFrontend& operator=(const SDLFrontend&)  = delete;

        // move operators enabled, do not throw exceptions
        SDLFrontend(SDLFrontend&&)              noexcept = default;
        SDLFrontend& operator=(SDLFrontend&&)   noexcept = default;

        [[nodiscard]]
        bool processEvents() const;

        void clear() const;
        void drawDebugText(float x, float y, std::string_view text) const;
        void present() const;

    private:
        using SDLWindow     = chip8::graphics::sdl_wrappers::WindowPtr;
        using SDLRenderer   = chip8::graphics::sdl_wrappers::RendererPtr;
        using SDLTexture    = chip8::graphics::sdl_wrappers::TexturePtr;
        using SDLContext    = chip8::graphics::sdl_wrappers::SDLContext;

        SDLWindow window_;
        SDLRenderer renderer_;
        SDLTexture texture_;
        SDLContext context_;
    };
} // chip8::graphics::frontend
