#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Cursor.hpp>

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <tuple>

#include "Object.hpp"
#include "Player.hpp"
#include "Collision.hpp"
#include "Gun.hpp"
#include "FlashLight.hpp"

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

        std::unique_ptr<sf::RenderWindow> window{nullptr};
        std::unique_ptr<sf::View> view{nullptr};
        std::unique_ptr<sf::Cursor> cursor{nullptr};

        sf::RenderTexture darknessTexture; // 암흑 텍스처 복원
        sf::Vector2f camPos{0.f, 0.f};

        std::string windowTitle;

        std::uint16_t screenWidth;
        std::uint16_t screenHeight;

        float deltaTime = 0.f;
        float zoomLevel = 1.0f;
        float lastUserSetRadius = 0.0f;

        bool isFollowingPlayer = false;

        sf::Vector2f lastMouseDirection{1.0f, 0.0f}; // 마우스 방향 저장용

        void handleEvents();
        void update();
        void render();
        void mouseCursorRender(Gun* gun);
        
        void handleCollisions();
        void renderDarknessEffect(); // 암흑 효과 렌더링 함수 복원

        sf::Vector2f findMapIntersection(const sf::Vector2f& start, const sf::Vector2f& direction, 
            float maxDistance, Collision* mapCollision);
    };
}