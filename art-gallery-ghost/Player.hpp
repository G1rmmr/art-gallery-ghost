#pragma once

#include <SFML/Graphics.hpp>

#include "Object.hpp"

#include <memory>
#include <vector>

class Player : public core::Object {
public:
    constexpr static sf::Color PLAYER_COLOR = sf::Color::Black;
    constexpr static float SHAPE_RADIUS = 30.f;
    constexpr static float MOVE_SPEED = 300.f;
    constexpr static float ZOOM_SPEED = 1.f;

    Player() = default;
    Player(const float x, const float y);

    void Update(const float deltaTime) override;
    
    void RenderComponents(sf::RenderWindow& window);
    void RenderGun(sf::RenderWindow& window);
    void RenderFlashLight(sf::RenderWindow& window);
};