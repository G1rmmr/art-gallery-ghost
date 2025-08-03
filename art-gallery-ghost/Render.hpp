#pragma once

#include <SFML/Graphics.hpp>

#include "Component.hpp"

class Render : public core::Component {
public:
    Render(core::Object* obj, std::unique_ptr<sf::Drawable> shape)
        : core::Component(obj)
        , shape(std::move(shape)) {}

    void Update(const float deltaTime) override {}

    std::string_view GetTag() const override { return tag; }

    template <typename T>
    T* GetShape() const { return dynamic_cast<T*>(shape.get()); }

    void Draw(sf::RenderWindow& window) {
        if(shape) window.draw(*shape);
    }

private:
    constexpr static std::string_view tag = "render";

    std::unique_ptr<sf::Drawable> shape;
};