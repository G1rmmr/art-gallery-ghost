#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

namespace engine
{
    class Object
    {
    public:
        Object() = default;
        virtual ~Object() {};

        virtual void Init() = 0;
        virtual void Update(const float deltaTime) = 0;
        virtual void Render(sf::RenderWindow& window) = 0;

    protected:
        sf::Color Color;
    };
}