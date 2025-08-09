#include "Game.hpp"

#include "Map.hpp"
#include "Controller.hpp"
#include "Render.hpp"
#include "Movement.hpp"
#include "Gun.hpp"
#include "FlashLight.hpp"

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

            else if(keyPressed->scancode == sf::Keyboard::Scan::R) {
                auto gun = std::dynamic_pointer_cast<Gun>(player->GetComponent("gun").lock());
                if(gun) gun->Reload();
            }

            else if(keyPressed->scancode == sf::Keyboard::Scan::Space)
                isFollowingPlayer = true;
        }
        else if(const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            if(keyReleased->scancode == sf::Keyboard::Scan::Space) 
                isFollowingPlayer = false;
        }
        else if(const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if(mousePressed->button == sf::Mouse::Button::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
                sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);

                auto gun = std::dynamic_pointer_cast<Gun>(player->GetComponent("gun").lock());
                if(gun) gun->Fire(worldMousePos);
            }

            if(mousePressed->button == sf::Mouse::Button::Right) {
                auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
                if(flashlight) flashlight->ToggleSwitch();
            }
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
                else {
                    auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
                    if(flashlight && flashlight->GetSwitch()) {
                        flashlight->AdjustRadius(delta);
                        flashlight->AdjustWidth(-delta);
                        flashlight->AdjustAlpha(static_cast<int>(delta));
                    }
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

    if(playerMovement) {
        if(isFollowingPlayer) {
            sf::Vector2f playerPos = playerMovement->GetPos();
            sf::Vector2f direction = playerPos - camPos;

            camPos += direction * FOLLOW_SPEED * deltaTime;

            float smoothing = std::exp(-FOLLOW_SPEED * deltaTime);
            camPos = camPos * smoothing + playerPos * (1.0f - smoothing);

            view->setCenter(camPos);
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);
        sf::Vector2f direction = worldMousePos 
            - (playerMovement->GetPos() + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS));

        float angle = std::atan2(direction.y, direction.x) * 180.0f / PI;
        
        auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
        if(flashlight) flashlight->SetAngles(angle);
    }

    window->setView(*view);
}

void Game::render() {
    for(const auto& object : objects) {
        if(auto render = std::dynamic_pointer_cast<Render>(
            object->GetComponent("render").lock())) {
            render->Draw(*window);
        }
    }

    std::dynamic_pointer_cast<Render>(
        player->GetComponent("render").lock())->Draw(*window);

    auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
    if(flashlight && flashlight->GetSwitch()) {
        flashlight->Render(*window);
    }

    auto gun = std::dynamic_pointer_cast<Gun>(player->GetComponent("gun").lock());
    if(gun) {
        if(gun->HasActiveBullets())
            gun->Render(*window);

        mouseCursorRender(gun.get());
    }
}

void Game::mouseCursorRender(Gun * gun) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2f worldMousePos = window->mapPixelToCoords(mousePos);

    const float GAUGE_RADIUS = 20.f;
    const float GAUGE_THICKNESS = 4.f;
    const int POINT_COUNT = 60;

    int currentAmmo = gun->GetAmmo();
    float ammoRatio = static_cast<float>(currentAmmo) / Gun::MAX_AMMO;

    sf::VertexArray gauge(sf::PrimitiveType::TriangleStrip, (static_cast<std::size_t>(POINT_COUNT * ammoRatio) + 1) * 2);

    sf::Color gaugeColor = sf::Color(
        static_cast<std::uint8_t>(255 * (1.0f - ammoRatio)),
        static_cast<std::uint8_t>(255 * ammoRatio),
        0,
        200
    );

    for(int i = 0; i <= POINT_COUNT * ammoRatio; ++i) {
        float angle = (static_cast<float>(i) / POINT_COUNT) * 2.0f * PI - PI / 2.0f;

        sf::Vector2f outerPoint(
            worldMousePos.x + std::cos(angle) * (GAUGE_RADIUS + GAUGE_THICKNESS / 2.0f),
            worldMousePos.y + std::sin(angle) * (GAUGE_RADIUS + GAUGE_THICKNESS / 2.0f)
        );

        sf::Vector2f innerPoint(
            worldMousePos.x + std::cos(angle) * (GAUGE_RADIUS - GAUGE_THICKNESS / 2.0f),
            worldMousePos.y + std::sin(angle) * (GAUGE_RADIUS - GAUGE_THICKNESS / 2.0f)
        );

        gauge[static_cast<size_t>(i) * 2].position = outerPoint;
        gauge[static_cast<size_t>(i) * 2].color = gaugeColor;
        gauge[static_cast<size_t>(i) * 2 + 1].position = innerPoint;
        gauge[static_cast<size_t>(i) * 2 + 1].color = gaugeColor;
    }

    window->draw(gauge);
}