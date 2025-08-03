#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "Player.hpp"
#include "Object.hpp"
#include "FlashLight.hpp"

namespace core {
    class Game {
    public:
        Game(const std::string& title,
            const std::uint16_t width,
            const std::uint16_t height);

        ~Game() = default;

        void Run();
        void Clear();

    private:
        std::vector<std::unique_ptr<Object>> objects;

        std::unique_ptr<Player> player{nullptr};
        std::unique_ptr<FlashLight> flash{nullptr};

        std::unique_ptr<sf::RenderWindow> window{nullptr};

        std::string windowTitle;

        std::uint16_t screenWidth;
        std::uint16_t screenHeight;

        float deltaTime = 0.f;

        void handleEvents();
        void update();
        void render();
    };
}