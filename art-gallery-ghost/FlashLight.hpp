#pragma once

#include <SFML/Graphics.hpp>

#include "Object.hpp"

#include <memory>
#include <vector>

class FlashLight : public core::Object {
public:
    constexpr static sf::Color FLASH_COLOR = sf::Color{255, 215, 0, 128};
    constexpr static float SHAPE_RADIUS = 300.f;

    FlashLight() = default;
    FlashLight(const float x, const float y, const float start, const float end);

    void Update(const float deltaTime) override;

    void SetAngles(const float start, const float end) {
        startAngle = start;
        endAngle = end;
    };

private:
    float startAngle;
    float endAngle;

    sf::VertexArray getVertices(const sf::Vector2f pos, const sf::Color& color) const;
};