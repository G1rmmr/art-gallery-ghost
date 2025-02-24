#pragma once

#include <SFML/Graphics.hpp>

namespace engine
{
    class Controller
    {
    public:
        Controller(const sf::Vector2f& center, const sf::Vector2f& size);

        void HandleInput(const float deltaTime);

        inline const sf::View& GetView() const
        {
            return mView;
        }

        inline void SetMoveSpeed(const float speed)
        {
            mMoveSpeed = speed;
        }

        inline void SetZoomSpeed(const float speed)
        {
            mZoomSpeed = speed;
        }

    private:
        sf::View mView;

        float mMoveSpeed;
        float mZoomSpeed;
    };
}