#include "frontend.h"
#include <stdexcept>

#include <iostream>

namespace chip8::graphics::frontend
{
    SDLFrontend::SDLFrontend(int width, int height, std::string_view title) :
        window_(),
        renderer_(),
        texture_(),
        context_(SDL_INIT_VIDEO)
    {
        SDL_Log("Initializing SDL frontend...");

        SDL_Window* window      = nullptr;
        SDL_Renderer* renderer  = nullptr;

        if (!SDL_CreateWindowAndRenderer(
            title.data(),
            width, height,
            0,
            &window, &renderer))
        {
            SDL_Quit();
            throw std::runtime_error(SDL_GetError());
        }

        SDL_Log("Renderer: %s", SDL_GetRendererName(renderer));

        window_.reset(window);
        renderer_.reset(renderer);
    }

    SDLFrontend::~SDLFrontend() noexcept
    {
        SDL_Log("Destructing SDLFrontend.");

        renderer_.reset();
        window_.reset();
        texture_.reset();
        SDL_Quit();
    }

    bool SDLFrontend::processEvents() const
    {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    return false;
                }
                default:
                {
                    break;
                }
            }
        }

        return true;
    }

    void SDLFrontend::clear() const
    {
        if (!SDL_SetRenderDrawColor(
            renderer_.get(),
            0, 0, 0,
            SDL_ALPHA_OPAQUE
        )) {
            throw std::runtime_error(SDL_GetError());
        }

        if (!SDL_RenderClear(renderer_.get())) {
            throw std::runtime_error(SDL_GetError());
        }

    }

    void SDLFrontend::drawDebugText(float x, float y, std::string_view text) const
    {
        if (!SDL_SetRenderDrawColor(
            renderer_.get(),
            255, 255, 255,
            SDL_ALPHA_OPAQUE
        )) {
            throw std::runtime_error(SDL_GetError());
        }

        if (!SDL_RenderDebugText(
            renderer_.get(),
            x, y,
            text.data()
        )) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void SDLFrontend::present() const
    {
        if (!SDL_RenderPresent(renderer_.get())) {
            throw std::runtime_error(SDL_GetError());
        }
    }
} // chip8::graphics::frontend