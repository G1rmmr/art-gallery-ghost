#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <memory>

#include "Map.hpp"

namespace engine
{
    class Game
    {
    public:
        Game(
            const std::string& title,
            const std::uint16_t width,
            const std::uint16_t height);

        ~Game() = default;

        void Run();
        void Clear();

    private:
        void init();
        void update(const float deltaTime);
        void render();

    private:
        std::unique_ptr<sf::RenderWindow> mWindow;

        std::string mWindowTitle;

        std::uint16_t mScreenWidth;
        std::uint16_t mScreenHeight;

        std::unique_ptr<Map> mMap;
    };
}