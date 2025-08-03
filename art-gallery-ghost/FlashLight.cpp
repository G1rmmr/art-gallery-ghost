#include "FlashLight.hpp"

#include "Controller.hpp"
#include "Movement.hpp"
#include "Render.hpp"

using namespace core;

constexpr float PI = 3.141592f;
constexpr int POINT_COUNT = 30;

FlashLight::FlashLight(const float x, const float y, const float startAngle)
    : startAngle(startAngle) {
    this->AddComponent(std::make_unique<Movement>(
        this,
        sf::Vector2f{x, y}));

    this->AddComponent(std::make_unique<Render>(
        this,
        std::make_unique<sf::VertexArray>(getVertices({x, y}, FLASH_COLOR))));
}

void FlashLight::Update(const float deltaTime) {
    auto movement = std::dynamic_pointer_cast<Movement>(this->GetComponent("movement").lock());
    auto render = std::dynamic_pointer_cast<Render>(this->GetComponent("render").lock());

    if(movement && render) {
        sf::VertexArray* vertices = render->GetShape<sf::VertexArray>();
        sf::Color color = sf::Color(FLASH_COLOR.r, FLASH_COLOR.g, FLASH_COLOR.b, alpha);
        if(vertices) *vertices = getVertices(movement->GetPos(), color);
    }
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