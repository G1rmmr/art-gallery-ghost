#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Collision.hpp"
#include <memory>
#include <algorithm>
#include <functional>

class FlashLight : public core::Component{
public:
    constexpr static sf::Color FLASH_COLOR = sf::Color{255, 215, 0};
    constexpr static sf::Color OUTER_GLOW_COLOR = sf::Color(255, 225, 170, 100);
    constexpr static sf::Color INNER_CORE_COLOR = sf::Color(255, 250, 230, 180);
    constexpr static float MAX_RADIUS = 3000.f;
    constexpr static float MIN_RADIUS = 500.f;
    constexpr static float MAX_FAN_WIDTH = 100.f;
    constexpr static float MIN_FAN_WIDTH = 10.f;
    constexpr static std::uint8_t MAX_ALPHA = 255;
    constexpr static std::uint8_t MIN_ALPHA = 127;
    constexpr static std::uint8_t WHEEL_LEVEL = 12;

    using IntersectionFinder = std::function<sf::Vector2f(const sf::Vector2f&, const sf::Vector2f&, float, Collision*)>;

    FlashLight(core::Object * obj);

    void Update(const float deltaTime) override {};
    std::string_view GetTag() const override { return tag; }

    bool GetSwitch() const { return isSwitchOn; }
    void ToggleSwitch() { isSwitchOn = !isSwitchOn; }

    void SetWidth(const float width) {
        fanWidth = std::max(MIN_FAN_WIDTH, std::min(width, MAX_FAN_WIDTH));
    }

    float GetWidth() const { return fanWidth; }

    void AdjustWidth(const float delta) {
        fanWidth = std::max(MIN_FAN_WIDTH,
            std::min(fanWidth + delta * (MAX_FAN_WIDTH - MIN_FAN_WIDTH) / WHEEL_LEVEL, MAX_FAN_WIDTH));
    }

    void SetRadius(const float radius) {
        this->radius = std::max(MIN_RADIUS, std::min(radius, MAX_RADIUS));
    }

    float GetRadius() const { return radius; }

    void AdjustRadius(const float delta) {
        radius = std::max(MIN_RADIUS,
            std::min(radius + delta * (MAX_RADIUS - MIN_RADIUS) / WHEEL_LEVEL, MAX_RADIUS));
    }

    void SetAngles(const float start) {
        startAngle = start - fanWidth / 2.0f;
    }

    float GetAngles() const { return startAngle; }

    void SetAlpha(const std::uint8_t alpha) {
        this->alpha = std::max(MIN_ALPHA, std::min(alpha, MAX_ALPHA));
    }

    std::uint8_t GetAlpha() const { return alpha; }

    void AdjustAlpha(const int delta) {
        int newAlpha = static_cast<int>(alpha) + delta * (MAX_ALPHA - MIN_ALPHA) / WHEEL_LEVEL;
        alpha = std::max(static_cast<int>(MIN_ALPHA), std::min(newAlpha, static_cast<int>(MAX_ALPHA)));
    }

    void SetMapCollision(Collision* mapCollision, IntersectionFinder finder) {
        this->mapCollision = mapCollision;
        this->findIntersection = std::move(finder);
    }

    void SetCenterPosition(const sf::Vector2f& position) {
        centerPosition = position;
        useCustomCenter = true;
    }

    void UsePlayerCenter() {
        useCustomCenter = false;
    }

    sf::VertexArray GetLightGradient() const;
    sf::VertexArray GetInnerLight() const;
    sf::RenderStates GetLightBlendMode() const;
    sf::RenderStates GetInnerBlendMode() const;
    void Render(sf::RenderTarget& target) const;

private:
    constexpr static std::string_view tag = "flashlight";
    constexpr static int POINT_COUNT = 30;

    float fanWidth = MAX_FAN_WIDTH;
    float radius = MIN_RADIUS;
    float startAngle = 0.f;
    std::uint8_t alpha = MIN_ALPHA;
    bool isSwitchOn = false;
    Collision* mapCollision = nullptr;
    IntersectionFinder findIntersection;
    sf::Vector2f centerPosition{0.f, 0.f};
    bool useCustomCenter = false;

    sf::VertexArray createClippedWedge(float sizeScale, const sf::Color& baseColor) const;
    sf::VertexArray getVertices(const sf::Vector2f pos, const sf::Color& color) const;
};