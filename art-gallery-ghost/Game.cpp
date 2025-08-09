#include "Game.hpp"

#include "Map.hpp"
#include "Controller.hpp"
#include "Render.hpp"
#include "Movement.hpp"
#include "Collision.hpp"
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
    , screenHeight(height)
    , darknessTexture(sf::Vector2u(width, height))
{
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

    // FlashLight에 맵 충돌 정보와 교차점 탐색 함수 전달
    auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
    auto mapCollision = std::dynamic_pointer_cast<Collision>(objects[0]->GetComponent("collision").lock());
    if (flashlight && mapCollision) {
        flashlight->SetMapCollision(mapCollision.get(), 
            [this](const sf::Vector2f& start, const sf::Vector2f& dir, float dist, Collision* coll) {
                return this->findMapIntersection(start, dir, dist, coll);
            });
    }
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
                if(flashlight) 
                    flashlight->ToggleSwitch();
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
                        lastUserSetRadius = flashlight->GetRadius();
                        flashlight->AdjustWidth(-delta);
                        flashlight->AdjustAlpha(static_cast<int>(delta));
                    }
                }
            }
        }

        if(const auto controller = std::dynamic_pointer_cast<Controller>(
            player->GetComponent("controller").lock()))
            controller->HandleEvents();
    }
}

void Game::update() {
    for(const auto& object : objects)
        object->Update(deltaTime);

    player->Update(deltaTime);
    
    handleCollisions();
    
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

        // 월드 좌표계에서 마우스 위치 계산

        sf::Vector2i screenMousePos = sf::Mouse::getPosition(*window);
        sf::Vector2f worldMousePos = window->mapPixelToCoords(screenMousePos);
        sf::Vector2f playerPos = playerMovement->GetPos();
        sf::Vector2f playerCenter = playerPos + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
        
        // 플레이어에서 마우스로의 벡터 (이게 부채꼴 방향)
        sf::Vector2f playerToMouse = worldMousePos - playerCenter;
        float angle = std::atan2(playerToMouse.y, playerToMouse.x) * 180.0f / PI;

        auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
        if(flashlight) {
            flashlight->SetAngles(angle); // 마우스 방향으로 각도만 설정
            lastMouseDirection = playerToMouse;
        }
    }
    
    window->setView(*view);
}

void Game::handleCollisions() {
    auto playerCollision = std::dynamic_pointer_cast<Collision>(player->GetComponent("collision").lock());
    auto playerMovement = std::dynamic_pointer_cast<Movement>(player->GetComponent("movement").lock());
    auto gun = std::dynamic_pointer_cast<Gun>(player->GetComponent("gun").lock());
    auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());

    if(!playerCollision || !playerMovement) return;

    for(const auto& object : objects) {
        auto mapCollision = std::dynamic_pointer_cast<Collision>(object->GetComponent("collision").lock());
        if(!mapCollision) continue;

        sf::Vector2f currentPos = playerMovement->GetPos();
        sf::Vector2f playerCenter = currentPos + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);

        if(!mapCollision->ContainsPoint(playerCenter)) {
            sf::Vector2f closestBoundaryPoint = mapCollision->GetClosestPointOnBoundary(playerCenter);
            sf::Vector2f toPlayer = playerCenter - closestBoundaryPoint;
            float distanceToPlayer = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

            if(distanceToPlayer > 0.001f) {
                sf::Vector2f normalToPlayer = toPlayer / distanceToPlayer;

                sf::Vector2f safePlayerCenter = closestBoundaryPoint + normalToPlayer * (Player::SHAPE_RADIUS + 2.0f);

                if(!mapCollision->ContainsPoint(safePlayerCenter)) {
                    sf::Vector2f mapCenter = mapCollision->GetCenter();
                    sf::Vector2f toMapCenter = mapCenter - closestBoundaryPoint;
                    float distanceToCenter = std::sqrt(toMapCenter.x * toMapCenter.x + toMapCenter.y * toMapCenter.y);

                    if(distanceToCenter > 0.001f) {
                        sf::Vector2f normalToCenter = toMapCenter / distanceToCenter;
                        safePlayerCenter = closestBoundaryPoint + normalToCenter * (Player::SHAPE_RADIUS + 5.0f);
                    }
                }

                sf::Vector2f safePlayerPos = safePlayerCenter - sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);

                playerMovement->SetPos(safePlayerPos);
                sf::Vector2f currentVelocity = playerMovement->GetVel();
                sf::Vector2f tangent = sf::Vector2f(-normalToPlayer.y, normalToPlayer.x);
                float tangentSpeed = currentVelocity.x * tangent.x + currentVelocity.y * tangent.y;
                sf::Vector2f slideVelocity = tangent * tangentSpeed;
                playerMovement->SetVel(slideVelocity * 0.8f);
            }
        }
        else {
            sf::Vector2f velocity = playerMovement->GetVel();
            sf::Vector2f nextPos = currentPos + velocity * deltaTime;
            sf::Vector2f nextPlayerCenter = nextPos + sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);

            if(!mapCollision->ContainsPoint(nextPlayerCenter)) {
                sf::Vector2f closestBoundaryPoint = mapCollision->GetClosestPointOnBoundary(nextPlayerCenter);
                sf::Vector2f toBoundary = closestBoundaryPoint - playerCenter;
                float distanceToBoundary = std::sqrt(toBoundary.x * toBoundary.x + toBoundary.y * toBoundary.y);

                if(distanceToBoundary > 0.001f) {
                    sf::Vector2f normalToBoundary = toBoundary / distanceToBoundary;

                    float velocityAlongNormal = velocity.x * normalToBoundary.x + velocity.y * normalToBoundary.y;
                    if(velocityAlongNormal < 0) {
                        velocity = velocity - normalToBoundary * velocityAlongNormal;
                        playerMovement->SetVel(velocity);
                    }

                    float minDistance = Player::SHAPE_RADIUS + 1.0f;
                    if(distanceToBoundary < minDistance) {
                        sf::Vector2f pushVector = normalToBoundary * (minDistance - distanceToBoundary);
                        sf::Vector2f adjustedCenter = playerCenter + pushVector;
                        sf::Vector2f adjustedPos = adjustedCenter - sf::Vector2f(Player::SHAPE_RADIUS, Player::SHAPE_RADIUS);
                        playerMovement->SetPos(adjustedPos);
                    }
                }
            }
        }

        if(gun) {
            const auto& bullets = gun->GetBullets();
            for(size_t i = 0; i < bullets.size(); ++i) {
                if(!bullets[i].active) continue;

                sf::Vector2f bulletCenter = bullets[i].position + sf::Vector2f(Gun::BULLET_RADIUS, Gun::BULLET_RADIUS);

                if(!mapCollision->ContainsPoint(bulletCenter)) {
                    gun->DeactivateBullet(i);
                }
            }
        }
    }
}

void Game::render() {
    for(const auto& object : objects) {
        if(auto render = std::dynamic_pointer_cast<Render>(
            object->GetComponent("render").lock())) {
            render->Draw(*window);
        }
    }

    // 플레이어 렌더링
    std::dynamic_pointer_cast<Render>(
        player->GetComponent("render").lock())->Draw(*window);

    // 암흑 효과 렌더링
    renderDarknessEffect();

    // 플레이어 컴포넌트 렌더링 (총알 등)
    player->RenderComponents(*window);

    // 마우스 커서 렌더링
    auto gun = std::dynamic_pointer_cast<Gun>(player->GetComponent("gun").lock());
    if(gun)
    {
        sf::View originalView = window->getView();
        window->setView(window->getDefaultView());
        mouseCursorRender(gun.get());
        window->setView(originalView);
    }
}

void Game::renderDarknessEffect() {
    sf::View originalView = window->getView();
    window->setView(window->getDefaultView());

    auto flashlight = std::dynamic_pointer_cast<FlashLight>(player->GetComponent("flashlight").lock());
    if (!flashlight || !flashlight->GetSwitch()) {
        sf::RectangleShape fullscreen_rect(sf::Vector2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight)));
        fullscreen_rect.setFillColor(sf::Color(0, 0, 0, 230));
        window->draw(fullscreen_rect, sf::RenderStates(sf::BlendMultiply));
    }
    else {
        darknessTexture.clear(sf::Color(0, 0, 0, 230));
        darknessTexture.setView(originalView);
        
        flashlight->Render(darknessTexture);
        
        darknessTexture.display();

        sf::Sprite darkSprite(darknessTexture.getTexture());
        window->draw(darkSprite, sf::RenderStates(sf::BlendMultiply));
    }

    window->setView(originalView);
}

void Game::mouseCursorRender(Gun* gun) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
    sf::Vector2f screenMousePos = static_cast<sf::Vector2f>(mousePos);

    const float GAUGE_RADIUS = 20.f;
    const float GAUGE_THICKNESS = 4.f;
    const int POINT_COUNT = 60;

    int currentAmmo = gun->GetAmmo();
    float ammoRatio = static_cast<float>(currentAmmo) / Gun::MAX_AMMO;

    int numPointsToDraw = static_cast<int>(POINT_COUNT * ammoRatio);
    if (numPointsToDraw <= 0) return;

    sf::VertexArray gauge(sf::PrimitiveType::TriangleStrip, (static_cast<std::size_t>(numPointsToDraw) + 1) * 2);

    sf::Color gaugeColor = sf::Color(
        static_cast<std::uint8_t>(255 * (1.0f - ammoRatio)),
        static_cast<std::uint8_t>(255 * ammoRatio),
        0,
        200
    );

    float totalAngle = 2.0f * PI * ammoRatio;
    float startAngle = -PI / 2.0f - totalAngle / 2.0f;

    for (int i = 0; i <= numPointsToDraw; ++i) {
        float currentRatio = static_cast<float>(i) / numPointsToDraw;
        float angle = startAngle + currentRatio * totalAngle;

        sf::Vector2f outerPoint(
            screenMousePos.x + std::cos(angle) * (GAUGE_RADIUS + GAUGE_THICKNESS / 2.0f),
            screenMousePos.y + std::sin(angle) * (GAUGE_RADIUS + GAUGE_THICKNESS / 2.0f)
        );

        sf::Vector2f innerPoint(
            screenMousePos.x + std::cos(angle) * (GAUGE_RADIUS - GAUGE_THICKNESS / 2.0f),
            screenMousePos.y + std::sin(angle) * (GAUGE_RADIUS - GAUGE_THICKNESS / 2.0f)
        );

        gauge[static_cast<size_t>(i) * 2].position = outerPoint;
        gauge[static_cast<size_t>(i) * 2].color = gaugeColor;
        gauge[static_cast<size_t>(i) * 2 + 1].position = innerPoint;
        gauge[static_cast<size_t>(i) * 2 + 1].color = gaugeColor;
    }

    window->draw(gauge);
}

sf::Vector2f Game::findMapIntersection(const sf::Vector2f& start, const sf::Vector2f& direction,
    float maxDistance, Collision* mapCollision) {

    if(!mapCollision)
        return start + direction * maxDistance;

    // 시작점이 맵 내부인지 확인
    bool startInside = mapCollision->ContainsPoint(start);
    if(!startInside) {
        return start + direction * maxDistance;
    }

    // Ray casting with smaller steps for accuracy
    const float stepSize = 10.0f; // 더 작은 스텝으로 변경
    const int maxSteps = static_cast<int>(maxDistance / stepSize) + 1;

    for(int step = 1; step <= maxSteps; ++step) {
        float distance = step * stepSize;
        if(distance > maxDistance) distance = maxDistance;

        sf::Vector2f testPoint = start + direction * distance;
        bool pointInside = mapCollision->ContainsPoint(testPoint);

        if(!pointInside) {
            // 경계를 넘었으므로 이전 스텝과 현재 스텝 사이에서 정밀 탐색
            float prevDistance = (step - 1) * stepSize;

            // 이진 탐색으로 정확한 교차점 찾기
            for(int i = 0; i < 15; ++i) { // 더 정밀하게
                float midDistance = (prevDistance + distance) / 2.0f;
                sf::Vector2f midPoint = start + direction * midDistance;

                if(mapCollision->ContainsPoint(midPoint)) {
                    prevDistance = midDistance;
                }
                else {
                    distance = midDistance;
                }
            }

            return start + direction * prevDistance;
        }

        if(distance >= maxDistance) break;
    }

    return start + direction * maxDistance;
}