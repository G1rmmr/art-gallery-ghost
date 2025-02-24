#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "../Player.hpp"
#include "Object.hpp"

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
        std::vector<std::unique_ptr<Object>> mObjects;
        std::unique_ptr<class Player> mPlayer;

        std::unique_ptr<sf::RenderWindow> mWindow;

        std::string mWindowTitle;

        std::uint16_t mScreenWidth;
        std::uint16_t mScreenHeight;
    };
}