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

    sf::CircleShape shape(SHAPE_RADIUS);
    shape.setFillColor(PLAYER_COLOR);

    this->AddComponent(std::make_unique<Render>(
        this,
        std::make_unique<sf::CircleShape>(shape)));
}

void Player::Update(const float deltaTime) {
    this->components["movement"]->Update(deltaTime);

    auto movement = std::dynamic_pointer_cast<Movement>(this->GetComponent("movement").lock());
    auto render = std::dynamic_pointer_cast<Render>(this->GetComponent("render").lock());

    if(movement && render) {
        sf::CircleShape* circle = render->GetShape<sf::CircleShape>();
        if(circle) circle->setPosition(movement->GetPos());
    }
}