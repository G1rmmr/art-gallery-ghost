#include <cstdint>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>

#include "Map.hpp"
#include "Render.hpp"

const sf::Color MAP_COLOR = sf::Color::White;
const std::uint8_t EDGE_POINT = 6;

constexpr float PI = 3.141592f;

struct PolarPoint {
    sf::Vector2f Pos;
    float Angle;
};

void Map::generateRandomWalls() {
    std::vector<sf::Vector2f> points;
    generateRandomPoints(points);

    std::unique_ptr<sf::ConvexShape> convex = std::make_unique<sf::ConvexShape>();
    convex->setPointCount(EDGE_POINT);

    for(std::size_t i = 0; i < EDGE_POINT; ++i)
        convex->setPoint(i, points[i]);

    convex->setFillColor(MAP_COLOR);

    this->AddComponent(std::make_unique<Render>(this, std::move(convex)));
}

void Map::generateRandomPoints(std::vector<sf::Vector2f>& points) const {
    std::mt19937 gen{std::random_device{}()};
    std::uniform_real_distribution<float> noiseDist(-PI / 24.f, PI / 24.f);

    std::vector<PolarPoint> polarPoints;

    for(std::size_t i = 0; i < EDGE_POINT; ++i) {
        float rad = static_cast<float>(i) * (2.f * PI / EDGE_POINT) + noiseDist(gen);

        polarPoints.emplace_back(PolarPoint{
            size * sf::Vector2f{std::cos(rad), std::sin(rad)},
            rad});
    }

    std::sort(polarPoints.begin(), polarPoints.end(),
        [](const PolarPoint& lhs, const PolarPoint& rhs) {
        return lhs.Angle < rhs.Angle;
    });

    for(const auto& point : polarPoints)
        points.emplace_back(point.Pos);
}