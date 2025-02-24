#include "Game.hpp"

#include "../Map.hpp"
#include "Controller.hpp"

#include <iostream>

using namespace engine;

const float MAP_SIZE = 300.f;

const std::uint8_t FPS = 60;

Game::Game(
    const std::string& title,
    const std::uint16_t width,
    const std::uint16_t height)
    : mWindow(nullptr)
    , mPlayer(nullptr)
    , mWindowTitle(title)
    , mScreenWidth(width)
    , mScreenHeight(height)
{
    mWindow = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({mScreenWidth, mScreenHeight}),
        mWindowTitle);

    mWindow->setFramerateLimit(FPS);

    mObjects.emplace_back(std::make_unique<Map>(MAP_SIZE, MAP_SIZE));
    
    mPlayer = std::make_unique<Player>(
        static_cast<float>(mScreenWidth / 2),
        static_cast<float>(mScreenHeight / 2));
}

void Game::Run()
{
    constexpr float deltaTime = 1.f / FPS;

    const auto onClose = [&](const sf::Event::Closed&)
    {
        mWindow->close();
    };

    const auto onKeyPressed = [&](const sf::Event::KeyPressed& keyPressed)
    {
        if(keyPressed.scancode == sf::Keyboard::Scancode::Escape)
        {
            mWindow->close();
        }
    };

    while(mWindow->isOpen())
    {
        mWindow->handleEvents(onClose, onKeyPressed);
        update(deltaTime);

        mWindow->clear();
        render();

        mWindow->display();
    }
}

void Game::Clear()
{

}

void Game::init()
{
    
}

void Game::update(const float deltaTime)
{
    for(const auto& object : mObjects)
    {
        object->Update(deltaTime);
    }
    mPlayer->Update(deltaTime);
    std::cout << mPlayer->GetController().GetView().getCenter().x << '\n';
    mWindow->setView(mPlayer->GetController().GetView());
}

void Game::render()
{
    for(const auto& object : mObjects)
    {
        object->Render(*mWindow);
    }
    mPlayer->Render(*mWindow);
}