#include <iostream>

#include <SDL3/SDL.h>

int init_sdl()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Cannot initialize SDL3: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Hello World Chip8", 640, 480, 0);
    if (!window) {
        SDL_Log("Failed to create SDL3 window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Delay(5000);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main()
{
    std::cout << "Chip8 App, hello world" << '\n';
    init_sdl();
    return 0;
}