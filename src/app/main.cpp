#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "renderer/frontend.h"
#include "core/memory.h"

int run_frontend() {
    using Frontend       = chip8::graphics::frontend::SDLFrontend;
    constexpr int WIDTH  = 1280;
    constexpr int HEIGHT = 640;

    std::cout << "Running Chip8 Frontend..." << '\n';

    try {
        Frontend renderer {WIDTH, HEIGHT, "Chip-8 Emulator"};

        float dbPosX = 20.0F;
        float dbPosY = 20.0F;

        while (true) {
            if (!renderer.processEvents()) break;

            renderer.clear();
            renderer.drawDebugText(dbPosX, dbPosY, "Chip-8 Emulator");
            renderer.drawDebugText(dbPosX, (dbPosY * 2), "SDL3 working!");
            renderer.present();

            dbPosX = ((int) dbPosX % WIDTH) + 2.0F;

            SDL_Delay(10);
        }

        return 0;
    }
    catch (const std::out_of_range& exception) {
        std::cerr << "Fatal error: " << exception.what() << '\n';

        return 1;
    }
}

int main() {
    std::cout << "Chip8 Emulator console" << '\n';

    const auto mem {std::make_unique<chip8::memory::Memory>()};
    mem->clear();

    const auto  dump = mem->dump();
    std::size_t skip = 0;
    for (const auto& m: dump) {
        std::cout << "0x0" << std::hex << static_cast<int>(m) << ' ';
        if (skip++ >= 6) {
            std::cout << '\n';
            skip = 0;
        }
    }
    std::cout << '\n';

    using Memory = chip8::memory::Memory;

    try {
        mem->write<Memory::Byte>(0x0200, 0xAA);
        const auto byte_value = mem->read<Memory::Byte>(0x0200);
        std::cout
            << "Value written at 0x0200: "
            << std::hex
            << static_cast<int>(byte_value)
            << '\n';

        mem->write<Memory::Word>(0x0200, 0xFFFF);
        const auto word_value = mem->read<Memory::Word>(0x0201);
        std::cout
            << "Value written at 0x0200: 0x"
            << std::hex
            << static_cast<int>(word_value)
            << '\n';
    }
    catch (const std::out_of_range& e) {
        std::cerr
            << "Correctly throw and catch out of bounds or misaligned read / write:"
            << '\n'
            << e.what()
            << '\n';
    }

    run_frontend();

    return 0;
}
