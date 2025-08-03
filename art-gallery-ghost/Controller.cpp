#include "Controller.hpp"
#include "Movement.hpp"
#include "Object.hpp"
#include "Player.hpp"

#include <iostream>

using namespace core;

void Controller::HandleEvents() {
    if(auto movement = std::dynamic_pointer_cast<Movement>(
        owner->GetComponent("movement").lock())) {

        sf::Vector2f velocity{0.f, 0.f};

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
            velocity.y -= Player::MOVE_SPEED;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
            velocity.y += Player::MOVE_SPEED;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
            velocity.x -= Player::MOVE_SPEED;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
            velocity.x += Player::MOVE_SPEED;

        movement->SetVel(velocity);
    }
}
