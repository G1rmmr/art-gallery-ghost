#include "Gun.hpp"

#include "Controller.hpp"
#include "Movement.hpp"
#include "Render.hpp"

#include <iostream>
#include <cmath>

using namespace core;

Gun::Gun(const float x, const float y) {
    this->AddComponent(std::make_unique<Movement>(
        this,
        sf::Vector2f{x, y}));

    this->AddComponent(std::make_unique<Controller>(this));

    sf::CircleShape shape(SHAPE_RADIUS);
    shape.setFillColor(BULLET_COLOR);

    this->AddComponent(std::make_unique<Render>(
        this,
        std::make_unique<sf::CircleShape>(shape)));
}

void Gun::Update(const float deltaTime) {
    auto movement = std::dynamic_pointer_cast<Movement>(this->GetComponent("movement").lock());
    auto render = std::dynamic_pointer_cast<Render>(this->GetComponent("render").lock());

    if(movement && render) {
        sf::Vector2f velocity = direction * MOVE_SPEED;
        movement->SetVel(velocity);
        movement->Update(deltaTime);

        sf::CircleShape* circle = render->GetShape<sf::CircleShape>();
        if(circle) circle->setPosition(movement->GetPos());
    }
}

void Gun::Fire(const sf::Vector2f target) {
    if(currAmmo == 0) {
        std::cout << "OUT OF AMMO!" << std::endl;
        return;
    }

    --currAmmo;

    auto pos = std::dynamic_pointer_cast<Movement>(this->GetComponent("movement").lock())->GetPos();
    sf::Vector2f diff = target - pos;
    
    float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);

    if(length > 0)
        direction = diff / length;
    
    nowFiring = true;
}