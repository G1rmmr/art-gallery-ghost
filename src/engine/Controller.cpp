#include "Controller.hpp"

#include <iostream>

using namespace engine;

const float MOVE_SPEED = 300.f;
const float ZOOM_SPEED = 1.f;

Controller::Controller(const sf::Vector2f& center, const sf::Vector2f& size)
    : mView(center, size)
    , mMoveSpeed(MOVE_SPEED)
    , mZoomSpeed(ZOOM_SPEED)
{

}

void Controller::HandleInput(const float deltaTime)
{
    sf::Vector2f movement(0.f, 0.f);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W))
    {
        movement.y -= mMoveSpeed * deltaTime;
    }
        
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S))
    {
        movement.y += mMoveSpeed * deltaTime;
    }
        
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A))
    {
        movement.x -= mMoveSpeed * deltaTime;
    }
        
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D))
    {
        movement.x += mMoveSpeed * deltaTime;
    }

    mView.move(movement);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
    {
        mView.zoom(1.f + mZoomSpeed * deltaTime);
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
    {
        mView.zoom(1.f - mZoomSpeed * deltaTime);
    }
}