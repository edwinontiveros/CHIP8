#include "frontend.h"
#include <exception>
#include <iostream>
#include <chrono>


int run_frontend()
{
    using Frontend = chip8::graphics::frontend::SDLFrontend;
    constexpr int WIDTH = 1280;
    constexpr int HEIGHT = 640;

    std::cout
        << "Running Chip8 Frontend..."
        << '\n';

    try {
        Frontend renderer{
            WIDTH, HEIGHT,
            "Chip-8 Emulator"
        };

        float dbPosX = 20.0f;
        float dbPosY = 20.0f;

        while (true) {
            if (!renderer.processEvents()) break;

            renderer.clear();

            renderer.drawDebugText(
                dbPosX, dbPosY,
                "Chip-8 Emulator"
            );

            renderer.drawDebugText(
                dbPosX, (dbPosY * 2),
                "SDL3 working!"
            );

            renderer.present();

            dbPosX = ((int)dbPosX % WIDTH) + 2.0f;

            SDL_Delay(60);
        }

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Fatal error: "
            << exception.what()
            << '\n';

        return 1;
    }
}

int main()
{
    std::cout
        << "Chip8 Emulator console"
        << '\n';

    int frontend_result = run_frontend();

    return frontend_result;
}