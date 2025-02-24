#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

#include "engine/Object.hpp"
#include "engine/Controller.hpp"

class Player : public engine::Object 
{
public:
    Player(const float x, const float y);
    ~Player() override;

    void Init() override;
    void Update(const float deltaTime) override;
    void Render(sf::RenderWindow& window) override;

    inline engine::Controller& GetController() const
    {
        return *mController;
    }

private:
    std::unique_ptr<engine::Controller> mController;
    sf::Vector2f mPos;
};