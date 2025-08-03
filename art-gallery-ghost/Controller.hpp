#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>

#include "Component.hpp"

class Controller : public core::Component{
public:
    Controller(core::Object * obj) : Component(obj) {}
    Controller(const Controller& other) : Component(other.owner) {}

    void operator=(const Controller& other) {
        this->owner = other.owner;
    }

    void Update(const float deltaTime) override {};
    void HandleEvents();
    std::string_view GetTag() const { return tag; }

private:
    std::string_view tag = "controller";
};