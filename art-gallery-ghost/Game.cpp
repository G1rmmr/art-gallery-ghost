#include "Game.hpp"

#include "Map.hpp"
#include "Controller.hpp"
#include "Render.hpp"
#include "Movement.hpp"

#include <iostream>
#include <algorithm>

using namespace core;

const float MAP_SIZE = 3000.f;
constexpr float PI = 3.141592f;

const std::uint8_t FPS = 60;

Game::Game(const std::string& title, const std::uint16_t width, const std::uint16_t height)
    : window(nullptr)
    , windowTitle(title)
    , screenWidth(width)
    , screenHeight(height) {
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({screenWidth, screenHeight}), windowTitle);

    deltaTime = 1.f / FPS;

    window->setFramerateLimit(FPS);
    window->setMouseCursorVisible(false);

    view = std::make_unique<sf::View>();
    view->setCenter({0.f, 0.f});
    view->setSize({
        static_cast<float>(screenWidth) * zoomLevel,
        static_cast<float>(screenHeight) * zoomLevel});

    window->setView(*view);

    objects.emplace_back(std::make_unique<Map>(MAP_SIZE));

    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);
    sf::Vector2f direction = worldMousePos - sf::Vector2f(0.f, 0.f);

    float angle = std::atan2(direction.y, direction.x) * 180.0f / PI;
    flash = std::make_unique<FlashLight>(0.f, 0.f, angle);

    player = std::make_unique<Player>(0.f, 0.f);
}

void Game::Run() {
    while(window->isOpen()) {
        handleEvents();
        update();

        window->clear();
        render();
        window->display();
    }
}

void Game::Clear() {

}

void Game::handleEvents() {
    while(const std::optional event = window->pollEvent()) {
        if(event->is<sf::Event::Closed>())
            window->close();

        if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if(keyPressed->scancode == sf::Keyboard::Scan::Escape)
                window->close();

            else if(keyPressed->scancode == sf::Keyboard::Scan::Space)
                isFollowingPlayer = true;
        }
        else if(const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            if(keyReleased->scancode == sf::Keyboard::Scan::Space) 
                isFollowingPlayer = false;
        }
        else if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if(mousePressed->button == sf::Mouse::Button::Right)
                flash->ToggleSwitch();
        }
        else if(const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
            if(mouseWheelScrolled->wheel == sf::Mouse::Wheel::Vertical) {
                float delta = mouseWheelScrolled->delta;

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift)) {
                    zoomLevel -= delta * ZOOM_SPEED;
                    zoomLevel = std::max(std::min(zoomLevel, MAX_ZOOM), MIN_ZOOM);

                    view->setSize({
                        static_cast<float>(screenWidth) * zoomLevel,
                        static_cast<float>(screenHeight) * zoomLevel});
                }
                else if(flash->GetSwitch()) {
                    flash->AdjustRadius(delta);
                    flash->AdjustWidth(-delta);
                    flash->AdjustAlpha(static_cast<int>(delta));
                }
            }
        }

        if(const auto controller = std::dynamic_pointer_cast<Controller>(
            player->GetComponent("controller").lock())) {
            controller->HandleEvents();
        }
    }
}

void Game::update() {
    for(const auto& object : objects)
        object->Update(deltaTime);

    player->Update(deltaTime);

    const auto playerMovement = std::dynamic_pointer_cast<Movement>(
        player->GetComponent("movement").lock());

    const auto flashMovement = std::dynamic_pointer_cast<Movement>(
        flash->GetComponent("movement").lock());

    if(playerMovement && flashMovement) {
        if(isFollowingPlayer) {
            sf::Vector2f playerPos = playerMovement->GetPos();
            sf::Vector2f direction = playerPos - camPos;

            camPos += direction * FOLLOW_SPEED * deltaTime;

            float smoothing = std::exp(-FOLLOW_SPEED * deltaTime);
            camPos = camPos * smoothing + playerPos * (1.0f - smoothing);

            view->setCenter(camPos);
        }

        flashMovement->SetPos(playerMovement->GetPos() +
            sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS));

        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);
        sf::Vector2f direction = worldMousePos - playerMovement->GetPos();

        float angle = std::atan2(direction.y, direction.x) * 180.0f / PI;
        flash->SetAngles(angle);
    }
    flash->Update(deltaTime);
    window->setView(*view);
}

void core::Game::render() {
    for(const auto& object : objects) {
        if(auto render = std::dynamic_pointer_cast<Render>(
            object->GetComponent("render").lock())) {
            render->Draw(*window);
        }
    }

    if(flash->GetSwitch())
        std::dynamic_pointer_cast<Render>(
            flash->GetComponent("render").lock())->Draw(*window);

    std::dynamic_pointer_cast<Render>(
        player->GetComponent("render").lock())->Draw(*window);
}