#include "Gun.hpp"
#include "Movement.hpp"
#include "Object.hpp"
#include "Player.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>

using namespace core;

void Gun::Update(const float deltaTime) {
    for(auto& bullet : bullets) {
        if(!bullet.active) continue;

        bullet.position += bullet.direction * BULLET_SPEED * deltaTime;
        bullet.lifetime -= deltaTime;

        if(bullet.lifetime <= 0.0f) {
            bullet.active = false;
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        bullets.end());
}

void Gun::Fire(const sf::Vector2f& target) {
    if(currAmmo <= 0) {
        std::cout << "OUT OF AMMO!" << std::endl;
        return;
    }

    --currAmmo;

    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    if(!movement) return;

    sf::Vector2f playerPos = movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
    sf::Vector2f direction = target - playerPos;
    
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if(length > 0) {
        direction = direction / length;
    }

    bullets.emplace_back(playerPos, direction);
}

void Gun::Render(sf::RenderWindow& window) const {
    sf::CircleShape bulletShape(BULLET_RADIUS);
    bulletShape.setFillColor(BULLET_COLOR);

    for(const auto& bullet : bullets) {
        if(bullet.active) {
            bulletShape.setPosition(bullet.position);
            window.draw(bulletShape);
        }
    }
}

bool Gun::HasActiveBullets() const {
    return std::any_of(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.active; });
}