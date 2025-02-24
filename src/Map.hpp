#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>

#include "Object.hpp"

class Map : public engine::Object
{
public:
    Map(const float width, const float height);
    ~Map() override;

    void Init() override;
    void Update(const float deltaTime) override;
    void Render(sf::RenderWindow& window) override;

private:
    void generateRandomWalls();

private:
    sf::Color mColor;

    float mMapWidth;
    float mMapHeight;
};