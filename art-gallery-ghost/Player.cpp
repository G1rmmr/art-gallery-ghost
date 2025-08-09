#include "Player.hpp"

#include "Controller.hpp"
#include "Movement.hpp"
#include "Render.hpp"
#include "Collision.hpp"
#include "Gun.hpp"
#include "FlashLight.hpp"

using namespace core;

Player::Player(const float x, const float y) {
    this->AddComponent(std::make_unique<Movement>(
        this,
        sf::Vector2f{x, y}));

    this->AddComponent(std::make_unique<Controller>(this));

    sf::CircleShape shape(SHAPE_RADIUS);
    shape.setFillColor(PLAYER_COLOR);

    this->AddComponent(std::make_unique<Render>(
        this,
        std::make_unique<sf::CircleShape>(shape)));

    this->AddComponent(std::make_unique<Collision>(this));

    this->AddComponent(std::make_unique<Gun>(this));
    this->AddComponent(std::make_unique<FlashLight>(this));
}

void Player::Update(const float deltaTime) {
    auto movement = std::dynamic_pointer_cast<Movement>(this->GetComponent("movement").lock());
    auto render = std::dynamic_pointer_cast<Render>(this->GetComponent("render").lock());

    if(movement && render) {
        sf::CircleShape* circle = render->GetShape<sf::CircleShape>();
        if(circle) circle->setPosition(movement->GetPos());
    }

    this->components["movement"]->Update(deltaTime);
    this->components["collision"]->Update(deltaTime);
    this->components["gun"]->Update(deltaTime);
    this->components["flashlight"]->Update(deltaTime);
}

void Player::RenderComponents(sf::RenderWindow& window) {
    RenderFlashLight(window);
    RenderGun(window);
}

void Player::RenderFlashLight(sf::RenderWindow& window) {
    auto flashlight = std::dynamic_pointer_cast<FlashLight>(this->GetComponent("flashlight").lock());
    if (!flashlight || !flashlight->GetSwitch()) return;

    sf::VertexArray lightGradient = flashlight->GetLightGradient();
    sf::VertexArray innerLight = flashlight->GetInnerLight();
    
    if (lightGradient.getVertexCount() > 0) {
        sf::RenderStates lightStates = flashlight->GetLightBlendMode();
        window.draw(lightGradient, lightStates);
    }
    
    if (innerLight.getVertexCount() > 0) {
        sf::RenderStates innerStates = flashlight->GetInnerBlendMode();
        window.draw(innerLight, innerStates);
    }
}

void Player::RenderGun(sf::RenderWindow& window) {
    auto gun = std::dynamic_pointer_cast<Gun>(this->GetComponent("gun").lock());
    if (!gun || !gun->HasActiveBullets()) return;

    const auto& bullets = gun->GetBullets();
    sf::CircleShape bulletShape = gun->GetBulletShape();
    
    for (const auto& bullet : bullets) {
        if (!bullet.active) continue;
        
        bulletShape.setPosition({bullet.position.x - Gun::BULLET_RADIUS,
                               bullet.position.y - Gun::BULLET_RADIUS});
        window.draw(bulletShape);
    }
}