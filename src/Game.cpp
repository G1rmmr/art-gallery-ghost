#include "Game.hpp"

using namespace engine;

const float MAP_SIZE = 100.f;
const std::uint8_t FPS = 60;

Game::Game(
    const std::string& title,
    const std::uint16_t width,
    const std::uint16_t height)
    : mWindow(nullptr)
    , mMap(nullptr)
    , mWindowTitle(title)
    , mScreenWidth(width)
    , mScreenHeight(height)
{
    mWindow = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({mScreenWidth, mScreenHeight}),
        mWindowTitle);

    mWindow->setFramerateLimit(60);

    mMap = std::make_unique<Map>(MAP_SIZE, MAP_SIZE);
}

void Game::Run()
{
    constexpr float deltaTime = 1 / FPS;
    
    while(mWindow->isOpen())
    {
        while(const std::optional event = mWindow->pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                mWindow->close();
            }
        }

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
    // TO-DO: update logics
}

void Game::render()
{
    mMap->Render(*mWindow);
}