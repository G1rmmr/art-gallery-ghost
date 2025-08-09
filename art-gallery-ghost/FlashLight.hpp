#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include <memory>
#include <algorithm>

class FlashLight : public core::Component{
public:
    constexpr static sf::Color FLASH_COLOR = sf::Color{255, 215, 0};
    constexpr static float MAX_RADIUS = 3000.f;
    constexpr static float MIN_RADIUS = 500.f;
    constexpr static float MAX_FAN_WIDTH = 100.f;
    constexpr static float MIN_FAN_WIDTH = 10.f;
    constexpr static std::uint8_t MAX_ALPHA = 255;
    constexpr static std::uint8_t MIN_ALPHA = 127;
    constexpr static std::uint8_t WHEEL_LEVEL = 12;

    FlashLight(core::Object * obj) : core::Component(obj) {}

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

    sf::VertexArray GetLightGradient() const;
    sf::VertexArray GetInnerLight() const;
    sf::Vector2f GetPosition() const;
    sf::RenderStates GetLightBlendMode() const;
    sf::RenderStates GetInnerBlendMode() const;

private:
    constexpr static std::string_view tag = "flashlight";
    constexpr static int POINT_COUNT = 30;

    float fanWidth = MAX_FAN_WIDTH;
    float radius = MIN_RADIUS;
    float startAngle = 0.f;
    std::uint8_t alpha = MIN_ALPHA;
    bool isSwitchOn = false;

    sf::VertexArray getVertices(const sf::Vector2f pos, const sf::Color& color) const;
};