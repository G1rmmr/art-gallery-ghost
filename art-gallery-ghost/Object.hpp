#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

#include "Component.hpp"

namespace core {
    class Object {
    public:
        virtual ~Object() = default;
        virtual void Update(const float deltaTime) = 0;

        void AddComponent(std::shared_ptr<Component> component) {
            components[std::string(component->GetTag())] = std::move(component);
        }

        std::weak_ptr<core::Component> GetComponent(const std::string_view tag) const {
            auto iter = components.find(std::string(tag));
            
            if(iter != components.end())
                return iter->second;

            return {};
        }

    protected:
        std::unordered_map<std::string, std::shared_ptr<Component>> components;
    };
}