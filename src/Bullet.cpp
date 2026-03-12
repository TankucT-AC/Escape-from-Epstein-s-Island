#include "Bullet.hpp"
#include <cmath>


Bullet::Bullet(const sf::Texture& InitTexture, 
    const sf::Vector2<float>& InitStartPos, 
    const sf::Vector2<float>& InitDirection,
    float InitSpeed,
    float InitLifetime) : Entity(InitTexture, InitSpeed), direction(InitDirection),
    lifetime(InitLifetime), dead(false)
{
    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    sprite.setPosition(InitStartPos);
}

void Bullet::update(const sf::Time& dt, sf::RenderWindow& window)
{
    sprite.move(direction * dt.asSeconds() * speed);

    lifetime -= dt.asSeconds();
    if (lifetime <= 0) 
    {
        dead = true;
        lifetime = 0;
    }
}

void Bullet::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

// Т.к. пуля всегда перемещается по заданной прямой, то мувать ее не надо
void Bullet::move(const sf::Vector2<float>& offset) {}

sf::Vector2<float> Bullet::getPosition() const { return sprite.getPosition(); }

int Bullet::getLifetime() const { return lifetime; }
bool Bullet::isBulletAlive() const { return !dead; }