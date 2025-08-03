#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>

#include "Component.hpp"

class Movement : public core::Component{
public:
    Movement(core::Object* obj) : Component(obj) {}

    Movement(core::Object* obj, const sf::Vector2f& pos)
        : Component(obj)
        , pos(pos)
        , velocity({0.f, 0.f}) {}

    Movement(const Movement& other) 
        : Component(other.owner)
        , pos(other.pos)
        , velocity(other.velocity) {}

    void operator=(const Movement& other) {
        this->owner = other.owner;
        this->pos = other.pos;
        this->velocity = other.velocity;
    }

    void Update(const float deltaTime) override { pos += deltaTime * velocity; }

    std::string_view GetTag() const override { return tag; }

    void SetPos(const sf::Vector2f& pos) { this->pos = pos; }
    void SetVel(const sf::Vector2f& vel) { this->velocity = vel; }

    sf::Vector2f GetPos() const { return pos; }
    sf::Vector2f GetVel() const { return velocity; }

private:
    constexpr static std::string_view tag = "movement";

    sf::Vector2f pos;
    sf::Vector2f velocity;
};