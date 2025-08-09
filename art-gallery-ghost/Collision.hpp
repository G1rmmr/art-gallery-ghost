#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "Movement.hpp"
#include "Render.hpp"
#include <memory>
#include <limits>

enum class CollisionType {
    Circle,
    Rectangle,
    Convex
};

struct CollisionInfo {
    bool hasCollision = false;
    sf::Vector2f penetrationVector{0.f, 0.f};
    float penetrationDepth = 0.f;
};

class Collision : public core::Component{
public:
    Collision(core::Object* obj) : Component(obj) {
        UpdateFromRenderShape();
    }

    void Update(const float deltaTime) override {
        UpdateFromRenderShape();
    }

    std::string_view GetTag() const override { return tag; }

    CollisionInfo CheckCollision(const Collision& other) const;

    bool ContainsPoint(const sf::Vector2f& point) const;

    sf::FloatRect GetBounds() const { return bounds; }

    CollisionType GetType() const { return type; }

    float GetRadius() const { return radius; }

    sf::Vector2f GetCenter() const { return center; }

    sf::Vector2f GetClosestPointOnBoundary(const sf::Vector2f& point) const;
    sf::Vector2f GetClosestPointOnLineSegment(const sf::Vector2f& point, const sf::Vector2f& lineStart, const sf::Vector2f& lineEnd) const;

private:
    constexpr static std::string_view tag = "collision";

    CollisionType type = CollisionType::Circle;
    sf::FloatRect bounds;
    sf::Vector2f center;
    float radius = 0.f;
    std::vector<sf::Vector2f> vertices;

    void UpdateFromRenderShape();

    CollisionInfo checkCircleCircle(const Collision& other) const;
    CollisionInfo checkCircleRect(const Collision& other) const;
    CollisionInfo checkRectRect(const Collision& other) const;
    CollisionInfo checkConvexCollision(const Collision& other) const;

    bool pointInCircle(const sf::Vector2f& point) const;
    bool pointInRect(const sf::Vector2f& point) const;
    bool pointInConvex(const sf::Vector2f& point) const;
};