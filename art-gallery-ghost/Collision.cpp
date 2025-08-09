#include "Collision.hpp"
#include "Object.hpp"

#include <cmath>
#include <algorithm>

using namespace core;

void Collision::UpdateFromRenderShape() {
    if (!owner) return;
    
    auto render = std::dynamic_pointer_cast<Render>(owner->GetComponent("render").lock());
    auto movement = std::dynamic_pointer_cast<Movement>(owner->GetComponent("movement").lock());
    
    if (!render) return;
    
    sf::Vector2f position{0.f, 0.f};

    bounds = sf::FloatRect(position, sf::Vector2f(radius * 2.f, radius * 2.f));

    if (movement)
        position = movement->GetPos();
    
    if (auto circle = render->GetShape<sf::CircleShape>()) {
        type = CollisionType::Circle;
        radius = circle->getRadius();
        center = position + sf::Vector2f{radius, radius};
        bounds = sf::FloatRect(position, sf::Vector2f(radius * 2.f, radius * 2.f));
    }
    else if (auto rect = render->GetShape<sf::RectangleShape>()) {
        type = CollisionType::Rectangle;
        sf::Vector2f size = rect->getSize();
        center = position + size * 0.5f;
        bounds = sf::FloatRect(position, size);
    }
    else if (auto convex = render->GetShape<sf::ConvexShape>()) {
        type = CollisionType::Convex;
        vertices.clear();
        
        std::size_t pointCount = convex->getPointCount();

        for (std::size_t i = 0; i < pointCount; ++i)
            vertices.emplace_back(convex->getPoint(i) + position);
        
        if (!vertices.empty()) {
            float minX = vertices[0].x, maxX = vertices[0].x;
            float minY = vertices[0].y, maxY = vertices[0].y;
            
            for (const auto& vertex : vertices) {
                minX = std::min(minX, vertex.x);
                maxX = std::max(maxX, vertex.x);
                minY = std::min(minY, vertex.y);
                maxY = std::max(maxY, vertex.y);
            }
            
            bounds = sf::FloatRect(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
            center = sf::Vector2f{(minX + maxX) * 0.5f, (minY + maxY) * 0.5f};
        }
    }
}

CollisionInfo Collision::CheckCollision(const Collision& other) const {
    if (!sf::FloatRect(bounds).findIntersection(other.bounds))
        return CollisionInfo{};
    
    if (type == CollisionType::Circle && other.type == CollisionType::Circle)
        return checkCircleCircle(other);

    else if (type == CollisionType::Circle && other.type == CollisionType::Rectangle) {
        return checkCircleRect(other);
    }
    else if (type == CollisionType::Rectangle && other.type == CollisionType::Circle) {
        CollisionInfo info = other.checkCircleRect(*this);
        info.penetrationVector = -info.penetrationVector;
        return info;
    }
    else if (type == CollisionType::Rectangle && other.type == CollisionType::Rectangle)
        return checkRectRect(other);
    else
        return checkConvexCollision(other);
}

CollisionInfo Collision::checkCircleCircle(const Collision& other) const {
    sf::Vector2f direction = other.center - center;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float combinedRadius = radius + other.radius;
    
    if (distance < combinedRadius) {
        float penetration = combinedRadius - distance;
        sf::Vector2f penetrationVec{0.f, 0.f};
        
        if (distance > 0.001f)
            penetrationVec = (direction / distance) * penetration;
        
        return CollisionInfo{true, penetrationVec, penetration};
    }
    
    return CollisionInfo{};
}

CollisionInfo Collision::checkCircleRect(const Collision& other) const {
    sf::Vector2f closest{
        std::max(other.bounds.position.x, std::min(center.x, other.bounds.position.x + other.bounds.size.x)),
        std::max(other.bounds.position.y, std::min(center.y, other.bounds.position.y + other.bounds.size.y))
    };
    
    sf::Vector2f direction = center - closest;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance < radius) {
        float penetration = radius - distance;
        sf::Vector2f penetrationVec{0.f, 0.f};

        penetrationVec = distance > 0.001f ? (direction / distance) * penetration : sf::Vector2f{0.f, -penetration};
        
        return CollisionInfo{true, penetrationVec, penetration};
    }
    
    return CollisionInfo{};
}

CollisionInfo Collision::checkRectRect(const Collision& other) const {
    float left = std::max(bounds.position.x, other.bounds.position.x);
    float right = std::min(bounds.position.x + bounds.size.x, other.bounds.position.x + other.bounds.size.x);
    float top = std::max(bounds.position.y, other.bounds.position.y);
    float bottom = std::min(bounds.position.y + bounds.size.y, other.bounds.position.y + other.bounds.size.y);
    
    if(left < right && top < bottom) {
        float intersectionWidth = right - left;
        float intersectionHeight = bottom - top;
        
        sf::Vector2f penetrationVec = intersectionWidth < intersectionHeight ?
            sf::Vector2f((center.x < other.center.x) ? -intersectionWidth : intersectionWidth, 0.f) :
            sf::Vector2f(0.f, (center.y < other.center.y) ? -intersectionHeight : intersectionHeight);
        
        
        float penetration = std::sqrt(penetrationVec.x * penetrationVec.x + penetrationVec.y * penetrationVec.y);
        return CollisionInfo{true, penetrationVec, penetration};
    }
    
    return CollisionInfo{};
}

CollisionInfo Collision::checkConvexCollision(const Collision& other) const {
    return checkRectRect(other);
}

bool Collision::ContainsPoint(const sf::Vector2f& point) const {
    switch (type) {
        case CollisionType::Circle:
            return pointInCircle(point);
        case CollisionType::Rectangle:
            return pointInRect(point);
        case CollisionType::Convex:
            return pointInConvex(point);
    }
    return false;
}

bool Collision::pointInCircle(const sf::Vector2f& point) const {
    sf::Vector2f diff = point - center;
    return (diff.x * diff.x + diff.y * diff.y) <= (radius * radius);
}

bool Collision::pointInRect(const sf::Vector2f& point) const {
    return bounds.contains(point);
}

bool Collision::pointInConvex(const sf::Vector2f& point) const {
    if (vertices.size() < 3) return false;
    
    int intersectionCount = 0;
    size_t j = vertices.size() - 1;
    
    for (size_t i = 0; i < vertices.size(); i++) {
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            intersectionCount++;
        }
        j = i;
    }
    
    return intersectionCount % 2 == 1;
}

sf::Vector2f Collision::GetClosestPointOnBoundary(const sf::Vector2f& point) const {
    if (type != CollisionType::Convex || vertices.empty()) {
        return center; // fallback
    }
    
    sf::Vector2f closestPoint = vertices[0];
    float minDistance = std::numeric_limits<float>::max();
    
    // 각 edge에 대해 가장 가까운 점 찾기
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t next = (i + 1) % vertices.size();
        sf::Vector2f edgeClosest = GetClosestPointOnLineSegment(point, vertices[i], vertices[next]);
        
        sf::Vector2f diff = point - edgeClosest;
        float distance = diff.x * diff.x + diff.y * diff.y;
        
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = edgeClosest;
        }
    }
    
    return closestPoint;
}

sf::Vector2f Collision::GetClosestPointOnLineSegment(const sf::Vector2f& point, const sf::Vector2f& lineStart, const sf::Vector2f& lineEnd) const {
    sf::Vector2f line = lineEnd - lineStart;
    sf::Vector2f toPoint = point - lineStart;
    
    float lineLength = line.x * line.x + line.y * line.y;
    if (lineLength < 0.001f) return lineStart;
    
    float t = std::max(0.0f, std::min(1.0f, (toPoint.x * line.x + toPoint.y * line.y) / lineLength));
    
    return lineStart + line * t;
}