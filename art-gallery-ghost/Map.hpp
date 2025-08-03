#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>

#include "Object.hpp"

class Map : public core::Object {
public:
    Map(const float size) : size(size) {
        generateRandomWalls();
    }

    void Update(const float deltaTime) override {};
private:
    float size = 0.f;

    void generateRandomWalls();
    void generateRandomPoints(std::vector<sf::Vector2f>& points) const;
};