#include "FlashLight.hpp"
#include "Movement.hpp"
#include "Object.hpp"
#include "Player.hpp"
#include "Collision.hpp"

#include <cmath>

using namespace core;

constexpr float PI = 3.141592f;

sf::VertexArray FlashLight::createClippedWedge(float sizeScale, const sf::Color& baseColor) const {
    sf::Vector2f worldCenter;
    
    if (useCustomCenter) {
        worldCenter = centerPosition;
    } else {
        auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
        if (!movement) return sf::VertexArray();
        worldCenter = movement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
    }
    
    const int pointCount = 60;
    float fanWidth = GetWidth() * PI / 180.0f;
    float maxDistance = GetRadius() * sizeScale;
    float directionAngle = GetAngles() * PI / 180.0f;

    float angleStep = fanWidth / static_cast<float>(pointCount - 1);

    sf::VertexArray wedge(sf::PrimitiveType::TriangleFan);
    
    sf::Color centerColor = baseColor;
    centerColor.a = std::min(static_cast<int>(baseColor.a) + 50, 255);
    wedge.append(sf::Vertex{worldCenter, centerColor});

    for (int i = 0; i < pointCount; ++i) {
        float angle = directionAngle + i * angleStep;
        sf::Vector2f rayDirection(std::cos(angle), std::sin(angle));
        
        // ⭐ 핵심: 맵 충돌 검사로 실제 교차점 계산
        sf::Vector2f intersection = findIntersection(worldCenter, rayDirection, maxDistance, mapCollision);

        float actualDistance = std::sqrt(std::pow(intersection.x - worldCenter.x, 2) + std::pow(intersection.y - worldCenter.y, 2));
        float normalizedDistance = (maxDistance > 0) ? actualDistance / maxDistance : 0;
        float attenuation = 1.0f - std::pow(normalizedDistance, 1.8f);

        sf::Color edgeColor = baseColor;
        edgeColor.a = static_cast<std::uint8_t>(baseColor.a * attenuation);

        if(normalizedDistance > 0.7f) {
            edgeColor.r = std::min(static_cast<int>(edgeColor.r) + 15, 255);
            edgeColor.g = std::max(static_cast<int>(edgeColor.g) - 10, 0);
        }
        
        wedge.append(sf::Vertex{intersection, edgeColor});
    }
    
    return wedge;
}

sf::VertexArray FlashLight::GetLightGradient() const {
    return createClippedWedge(0.8f, OUTER_GLOW_COLOR);
}

sf::VertexArray FlashLight::GetInnerLight() const {
    return createClippedWedge(0.5f, INNER_CORE_COLOR);
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

void FlashLight::Render(sf::RenderTarget& target) const {
    if (!GetSwitch()) return;
    target.draw(GetLightGradient(), GetLightBlendMode());
    target.draw(GetInnerLight(), GetInnerBlendMode());
}

FlashLight::FlashLight(core::Object * obj) : core::Component(obj) {}