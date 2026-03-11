#include "Bullet.hpp"
#include <cmath>
#include <memory>

Bullet::Bullet(const sf::Texture& texture, 
    const sf::Vector2<float>& startPos, 
    const sf::Vector2<float>& dir) : direction(dir)
{
    sprite = std::make_unique<sf::Sprite>(texture);
    sf::FloatRect b = sprite->getLocalBounds();
    sprite->setOrigin(b.width / 2.f, b.height / 2.f);
    sprite->setPosition(startPos);

    speed = 200.f;
    lifetime = 2.f;
    dead = false;
}

void Bullet::update(const sf::Time& dt)
{
    sprite->move(direction * dt.asSeconds() * speed);

    lifetime -= dt.asSeconds();
    if (lifetime <= 0) dead = true;
}

void Bullet::draw(sf::RenderWindow& window)
{
    window.draw(*sprite);
}

int Bullet::getLifetime() const { return lifetime; }
bool Bullet::isBulletAlive() const { return !dead; }