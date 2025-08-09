#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include <memory>
#include <vector>

namespace core {
    class Gun : public Component {
    public:
        constexpr static sf::Color BULLET_COLOR = sf::Color::Red;
        constexpr static float BULLET_RADIUS = 5.f;
        constexpr static float BULLET_SPEED = 3000.f;
        constexpr static int MAX_AMMO = 10;
        constexpr static float BULLET_LIFETIME = 3.0f;

        struct Bullet {
            sf::Vector2f position;
            sf::Vector2f direction;
            float lifetime;
            bool active;

            Bullet(sf::Vector2f pos, sf::Vector2f dir) 
                : position(pos), direction(dir), lifetime(BULLET_LIFETIME), active(true) {}
        };

        Gun(Object* obj) : Component(obj) {}

        void Update(const float deltaTime) override;
        std::string_view GetTag() const override { return tag; }

        void Fire(const sf::Vector2f& target);
        void Render(sf::RenderWindow& window) const;

        bool HasActiveBullets() const;
        int GetAmmo() const { return currAmmo; }
        void Reload() { currAmmo = MAX_AMMO; }

    private:
        constexpr static std::string_view tag = "gun";
        
        std::vector<Bullet> bullets;
        int currAmmo = MAX_AMMO;
    };
}