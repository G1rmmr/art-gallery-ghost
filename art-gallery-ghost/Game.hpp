#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "Object.hpp"
#include "Player.hpp"
#include "FlashLight.hpp"
#include "Gun.hpp"

namespace core {
    class Game {
    public:
        const float MIN_ZOOM = 0.5f;
        const float MAX_ZOOM = 3.0f;
        const float ZOOM_SPEED = 0.1f;
        const float FOLLOW_SPEED = 5.0f;

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
        std::unique_ptr<Gun> gun{nullptr};

        std::unique_ptr<sf::RenderWindow> window{nullptr};
        std::unique_ptr<sf::View> view{nullptr};

        sf::Vector2f camPos{0.f, 0.f};

        std::string windowTitle;

        std::uint16_t screenWidth;
        std::uint16_t screenHeight;

        float deltaTime = 0.f;
        float zoomLevel = 1.0f;

        bool isFollowingPlayer = false;

        void handleEvents();
        void update();
        void render();
    };
}