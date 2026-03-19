#include "Bullet.hpp"
#include <SFML/System/Time.hpp>
#include <cmath>


Bullet::Bullet(const sf::Texture& InitTexture, 
    const sf::Vector2<float>& InitStartPos, 
    const sf::Vector2<float>& InitDirection,
    float InitSpeed,
    float InitLifetime,
    float InitDamage) : Entity(InitTexture, InitSpeed), direction(InitDirection),
    lifetime(InitLifetime), damage(InitDamage), dead(false)
{
    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    sprite.setPosition(InitStartPos);
    sprite.setScale(0.5f, 0.5f);
}

void Bullet::update(const sf::Time& dt, sf::RenderWindow& window)
{
    auto offset = direction * speed;
    this->move(dt, offset);

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

void Bullet::move(const sf::Time& dt, const sf::Vector2<float>& offset) 
{
    sprite.move(dt.asSeconds() * offset);
}

sf::Vector2<float> Bullet::getPosition() const { return sprite.getPosition(); }

int Bullet::getLifetime() const { return lifetime; }
float Bullet::getDamage() const { return damage; }
bool Bullet::isBulletAlive() const { return !dead; }

sf::Rect<float> Bullet::getHitbox() const { return sprite.getGlobalBounds(); }