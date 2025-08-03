#pragma once

#include <SFML/Graphics.hpp>

#include "Object.hpp"

#include <memory>
#include <vector>
#include <algorithm>

class FlashLight : public core::Object {
public:
    constexpr static sf::Color FLASH_COLOR = sf::Color{255, 215, 0, };
    constexpr static float MAX_RADIUS = 1000.f;
    constexpr static float MIN_RADIUS = 500.f;

    constexpr static float MAX_FAN_WIDTH = 100.f;
    constexpr static float MIN_FAN_WIDTH = 30.f;

    constexpr static std::uint8_t MAX_ALPHA = 255.f;
    constexpr static std::uint8_t MIN_ALPHA = 64.f;

    FlashLight() = default;
    FlashLight(const float x, const float y, const float startAngle);

    void Update(const float deltaTime) override;

    bool GetSwitch() const { return isSwitchOn; }

    void SetWidth(const float width) {
        fanWidth = std::max(MIN_FAN_WIDTH, std::min(width, MAX_FAN_WIDTH));
    }

    void AdjustWidth(const float delta) {
        fanWidth = std::max(MIN_FAN_WIDTH, std::min(fanWidth + delta, MAX_FAN_WIDTH));
    }

    void SetRadius(const float radius) {
        this->radius = std::max(MIN_RADIUS, std::min(radius, MAX_RADIUS));
    }

    void AdjustRadius(const float delta) {
        radius = std::max(MIN_RADIUS, std::min(radius + delta, MAX_RADIUS));
    }

    void SetAngles(const float start) {
        startAngle = start - fanWidth / 2.0f;
    }

    void SetAlpha(const std::uint8_t alpha) {
        this->alpha = std::max(MIN_ALPHA, std::min(alpha, MAX_ALPHA));
    }

    void AdjustAlpha(const int delta) {
        int newAlpha = static_cast<int>(alpha) + delta;
        alpha = std::max(static_cast<int>(MIN_ALPHA), std::min(newAlpha, static_cast<int>(MAX_ALPHA)));
    }

    void ToggleSwitch() { isSwitchOn = isSwitchOn ? false : true; }

private:
    float fanWidth = MIN_FAN_WIDTH;
    float radius = MIN_RADIUS;
    float startAngle = 0.f;

    std::uint8_t alpha = MIN_ALPHA;

    bool isSwitchOn = false;

    sf::VertexArray getVertices(const sf::Vector2f pos, const sf::Color& color) const;
};