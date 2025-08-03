#pragma once

namespace core {
    class Object;

    class Component {
    public:
        Component(Object* obj) : owner(obj) {}
        virtual ~Component() = default;
        
        virtual void Update(const float deltaTime) = 0;
        virtual std::string_view GetTag() const = 0;

    protected:
        Object* owner = nullptr;
    };
}