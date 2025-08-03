#include "Player.hpp"

#include "Controller.hpp"
#include "Movement.hpp"
#include "Render.hpp"

using namespace core;

Player::Player(const float x, const float y) {
    this->AddComponent(std::make_unique<Movement>(
        this,
        sf::Vector2f{x, y}));

    this->AddComponent(std::make_unique<Controller>(this));

    this->AddComponent(std::make_unique<Render>(
        this,
        PLAYER_COLOR,
        std::make_unique<sf::CircleShape>(SHAPE_RADIUS)));
}

void Player::Update(const float deltaTime) {
    for(auto& [_, component] : this->components)
        component->Update(deltaTime);
}