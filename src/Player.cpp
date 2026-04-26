#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include "UpdateContext.hpp"
#include "Room.hpp"

Player::Player(const sf::Texture& InitTexture, 
    sf::Vector2<float> InitPos,
    float InitSpeed,
    float InitShootDelay) : Entity(InitTexture, InitSpeed, InitPos), ShootTime(0.f),
    ShootDelay(InitShootDelay) {}

void Player::move(const sf::Time& dt, const sf::Vector2<float>& offset)
{
    sprite.move(dt.asSeconds() * offset);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);

    #if DEBUG_DRAW_COLLISIONS
        sf::FloatRect hb = this->getHitbox();
        sf::RectangleShape debugRect({hb.width, hb.height});
        debugRect.setPosition(hb.left, hb.top);
        debugRect.setFillColor(sf::Color::Transparent);
        debugRect.setOutlineColor(sf::Color::Red);
        debugRect.setOutlineThickness(1.f);
        window.draw(debugRect);
    #endif
}

void Player::update(const UpdateContext& ctx)
{
    // перемещение игрока в пространстве
    auto offset = sf::Vector2<float>(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) offset.y = -speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) offset.y = speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) offset.x = -speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) offset.x = speed;

    float length = std::sqrt(offset.x * offset.x + offset.y * offset.y);
    if (length > 0)
    {
        offset /= length;

        float step = speed * ctx.dt.asSeconds();
        sf::Vector2f oldPos = sprite.getPosition();

        sprite.move(offset.x * step, 0.f);
        if (ctx.room.checkCollision(*this)) {
            sprite.setPosition(oldPos.x, sprite.getPosition().y); 
        }

        // Пробуем по Y (уже с учетом возможного нового X)
        sf::Vector2f posAfterX = sprite.getPosition();
        sprite.move(0.f, offset.y * step);
        if (ctx.room.checkCollision(*this)) {
            sprite.setPosition(posAfterX.x, oldPos.y);
        }
    }

    //this->move(ctx.dt, offset);

    // вращение игрока в зависимости от позиции мышки
    sf::Vector2<int> mousePos = sf::Mouse::getPosition(ctx.window);
    sf::Vector2<float> worldMousePos = ctx.window.mapPixelToCoords(mousePos);
    auto playerPos = sprite.getPosition();
    
    sf::Vector2<float> direction = worldMousePos - playerPos;

    auto radians = static_cast<float>(std::atan2(direction.y, direction.x));

    auto degrees = radians * 180.f / config::PI;
    sprite.setRotation(degrees);
}

void Player::moveShootTime(const sf::Time& dt)
{
    ShootTime -= dt.asSeconds();
}

bool Player::isShootTime() const { return ShootTime <= 0; }

sf::Rect<float> Player::getHitbox() const 
{
    sf::Vector2<float> pos = sprite.getPosition();
    
    float hb_width = 24.f;
    float hb_height = 16.f;

    float offsetY = 20.f;

    return sf::Rect<float>(
        pos.x - hb_width / 2.f, 
        pos.y - hb_height / 2.f + offsetY, 
        hb_width, 
        hb_height
    );
}

void Player::cooldown() { ShootTime = ShootDelay; }