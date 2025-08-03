#pragma once

#include <SFML/Graphics.hpp>

#include "Component.hpp"

class Render : public core::Component {
public:
    Render(core::Object* obj, sf::Color color, std::unique_ptr<sf::Shape> shape)
        : core::Component(obj), shape(std::move(shape)) {
        this->shape->setFillColor(color);
    }

    void Update(const float deltaTime) override;

    std::string_view GetTag() const override { return tag; }

    void Draw(sf::RenderWindow& window) {
        if(shape) window.draw(*shape);
    }

private:
    std::unique_ptr<sf::Shape> shape;

    constexpr static std::string_view tag = "render";
};