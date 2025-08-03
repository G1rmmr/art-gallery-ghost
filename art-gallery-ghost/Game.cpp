#include "Game.hpp"

#include "Map.hpp"
#include "Controller.hpp"
#include "Render.hpp"
#include "Movement.hpp"

#include <iostream>

using namespace core;

const float MAP_SIZE = 300.f;

const std::uint8_t FPS = 60;

Game::Game(const std::string& title, const std::uint16_t width, const std::uint16_t height)
    : window(nullptr)
    , windowTitle(title)
    , screenWidth(width)
    , screenHeight(height) {
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({screenWidth, screenHeight}), windowTitle);

    sf::View view;
    view.setCenter({0.f, 0.f});
    view.setSize({static_cast<float>(width), static_cast<float>(height)});
    window->setView(view);

    deltaTime = 1.f / FPS;

    window->setFramerateLimit(FPS);

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
        }

        if(auto controller = std::dynamic_pointer_cast<Controller>(
            player->GetComponent("controller").lock())) {
            controller->HandleEvents();
        }
    }
}

void Game::update() {
    for(const auto& object : objects)
        object->Update(deltaTime);

    player->Update(deltaTime);
}

void core::Game::render() {
    for(const auto& object : objects) {
        if(auto render = std::dynamic_pointer_cast<Render>(
            object->GetComponent("render").lock())) {
            render->Draw(*window);
        }
    }

    std::dynamic_pointer_cast<Render>(
        player->GetComponent("render").lock())->Draw(*window);
}