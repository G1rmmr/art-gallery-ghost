#include "Map.hpp"

Map::Map(const float width, const float height)
    : mMapWidth(width)
    , mMapHeight(height)
    , mColor(sf::Color::White)
{
    
}

Map::~Map()
{

}

void Map::Init()
{

}

void Map::Update(const float deltaTime)
{

}

void Map::Render(sf::RenderWindow& window)
{
    sf::RectangleShape shape({mMapWidth, mMapHeight});
    shape.setFillColor(mColor);
    window.draw(shape);
}

void Map::generateRandomWalls()
{

}