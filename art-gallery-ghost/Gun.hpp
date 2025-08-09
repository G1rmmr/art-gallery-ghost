#pragma once

#include <SFML/Graphics.hpp>

#include "Object.hpp"

#include <memory>
#include <vector>

class Gun : public core::Object {
public:
    constexpr static sf::Color BULLET_COLOR = sf::Color::Red;

    constexpr static float SHAPE_RADIUS = 5.f;
    constexpr static float MOVE_SPEED = 3000.f;

    constexpr static int MAX_AMMO = 10;

    Gun() = default;
    Gun(const float x, const float y);

    void Update(const float deltaTime) override;
    void Fire(const sf::Vector2f target);

    bool GetFiring() const { return nowFiring; }
    void SetFiring(const bool nowFiring) { this->nowFiring = nowFiring; }

    int GetAmmo() const { return currAmmo; }

    void Reload() { currAmmo = MAX_AMMO; }

private:
    sf::Vector2f direction;
    int currAmmo = MAX_AMMO;
    bool nowFiring = false;
};