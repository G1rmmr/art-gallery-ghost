#include "Render.hpp"
#include "Object.hpp"
#include "Movement.hpp"

#include <memory>

void Render::Update(const float deltaTime) {
    if(auto movement = std::dynamic_pointer_cast<Movement>(
        owner->GetComponent("movement").lock())) {
        shape->setPosition(movement->GetPos());
    }
}
