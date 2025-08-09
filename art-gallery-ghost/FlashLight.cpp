#include "FlashLight.hpp"
#include "Movement.hpp"
#include "Object.hpp"
#include "Player.hpp"

#include <cmath>

using namespace core;

constexpr float PI = 3.141592f;

void FlashLight::Render(sf::RenderWindow& window) const {
    if(!isSwitchOn) return;

    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    if(!movement) return;

    sf::Vector2f pos = movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
    sf::Color color = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, alpha);
    
    sf::VertexArray vertices = getVertices(pos, color);
    window.draw(vertices);
}

sf::VertexArray FlashLight::getVertices(const sf::Vector2f pos, const sf::Color& color) const {
    sf::VertexArray vertices(sf::PrimitiveType::TriangleFan);
    vertices.append({pos, color});

    const float angleStep = fanWidth / static_cast<float>(POINT_COUNT - 1);

    for(unsigned int i = 1; i < POINT_COUNT; ++i) {
        float angle = startAngle + (i - 1) * angleStep;
        float x = pos.x + radius * std::cos(angle * PI / 180.0f);
        float y = pos.y + radius * std::sin(angle * PI / 180.0f);

        vertices.append({{x, y}, color});
    }
    
    return vertices;
}