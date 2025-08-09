#include "FlashLight.hpp"
#include "Movement.hpp"
#include "Object.hpp"
#include "Player.hpp"

#include <cmath>

using namespace core;

constexpr float PI = 3.141592f;

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

sf::VertexArray FlashLight::GetLightGradient() const {
    if(!isSwitchOn) return sf::VertexArray();

    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    if(!movement) return sf::VertexArray();

    sf::Vector2f pos = movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
    
    sf::VertexArray lightGradient(sf::PrimitiveType::TriangleFan);
    
    sf::Color centerColor = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, alpha);
    lightGradient.append({pos, centerColor});

    const float angleStep = fanWidth / static_cast<float>(POINT_COUNT - 1);

    for(unsigned int i = 1; i < POINT_COUNT; ++i) {
        float angle = startAngle + (i - 1) * angleStep;
        float x = pos.x + radius * std::cos(angle * PI / 180.0f);
        float y = pos.y + radius * std::sin(angle * PI / 180.0f);

        sf::Color edgeColor = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, 0);
        lightGradient.append({{x, y}, edgeColor});
    }
    
    return lightGradient;
}

sf::VertexArray FlashLight::GetInnerLight() const {
    if(!isSwitchOn) return sf::VertexArray();

    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    if(!movement) return sf::VertexArray();

    sf::Vector2f pos = movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
    
    sf::VertexArray innerLight(sf::PrimitiveType::TriangleFan);
    
    float innerRadius = radius * 0.4f;
    std::uint8_t innerAlpha = std::min(static_cast<int>(alpha) + 50, 255);
    
    sf::Color innerCenterColor = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, innerAlpha);
    innerLight.append({pos, innerCenterColor});

    const float angleStep = fanWidth / static_cast<float>(POINT_COUNT - 1);

    for(unsigned int i = 1; i < POINT_COUNT; ++i) {
        float angle = startAngle + (i - 1) * angleStep;
        float x = pos.x + innerRadius * std::cos(angle * PI / 180.0f);
        float y = pos.y + innerRadius * std::sin(angle * PI / 180.0f);

        sf::Color innerEdgeColor = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, innerAlpha / 3);
        innerLight.append({{x, y}, innerEdgeColor});
    }
    
    return innerLight;
}

sf::Vector2f FlashLight::GetPosition() const {
    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    if(!movement) return sf::Vector2f(0.f, 0.f);
    
    return movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
}

sf::RenderStates FlashLight::GetLightBlendMode() const {
    sf::RenderStates lightStates;
    lightStates.blendMode = sf::BlendMode(
        sf::BlendMode::Factor::SrcAlpha,
        sf::BlendMode::Factor::One,
        sf::BlendMode::Equation::Add,
        sf::BlendMode::Factor::Zero,
        sf::BlendMode::Factor::One,
        sf::BlendMode::Equation::Add
    );
    return lightStates;
}

sf::RenderStates FlashLight::GetInnerBlendMode() const {
    sf::RenderStates innerLightStates;
    innerLightStates.blendMode = sf::BlendAdd;
    return innerLightStates;
}