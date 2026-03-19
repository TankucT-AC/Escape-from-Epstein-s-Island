#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>

Player::Player(const sf::Texture& InitTexture, 
    float InitSpeed,
    float InitShootDelay) : Entity(InitTexture, InitSpeed), ShootTime(0.f),
    ShootDelay(InitShootDelay)
{
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Player::move(const sf::Time& dt, const sf::Vector2<float>& offset)
{
    sprite.move(dt.asSeconds() * offset);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Player::update(const sf::Time& dt, sf::RenderWindow& window)
{
    // перемещение игрока в пространстве
    auto offset = sf::Vector2<float>(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) offset.y = -speed;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) offset.y = speed;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) offset.x = -speed;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) offset.x = speed;

    this->move(dt, offset);

    // вращение игрока в зависимости от позиции мышки
    sf::Vector2<int> mousePos = sf::Mouse::getPosition(window);
    sf::Vector2<float> worldMousePos = window.mapPixelToCoords(mousePos);
    auto playerPos = sprite.getPosition();
    
    sf::Vector2<float> direction = worldMousePos - playerPos;

    auto radians = static_cast<float>(std::atan2(direction.y, direction.x));

    auto degrees = radians * 180.f / 3.14159265f;
    sprite.setRotation(degrees);
}

sf::Vector2<float> Player::getPosition() const 
{
    return sprite.getPosition();
}

void Player::moveShootTime(const sf::Time& dt)
{
    ShootTime -= dt.asSeconds();
}

bool Player::isShootTime() const { return ShootTime <= 0; }

void Player::cooldown() { ShootTime = ShootDelay; }