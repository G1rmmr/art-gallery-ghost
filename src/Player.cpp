#include "Player.hpp"

using namespace engine;

Player::Player(const float x, const float y)
    : mController(nullptr)
    , mPos({x, y})
{
    mController = std::make_unique<Controller>(
        mPos, sf::Vector2f{640.f, 360.f});

    Color = sf::Color::Cyan;
}

Player::~Player()
{

}

void Player::Init()
{

}

void Player::Update(const float deltaTime)
{
    mController->HandleInput(deltaTime);
    mPos = mController->GetView().getCenter();
}

void Player::Render(sf::RenderWindow& window)
{
    sf::CircleShape shape(5.f);
    shape.setFillColor(Color);
    shape.setPosition(mPos);

    window.draw(shape);
}