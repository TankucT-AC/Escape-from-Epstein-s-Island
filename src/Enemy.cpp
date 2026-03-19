#include "Enemy.hpp"
#include <cmath>
#include "Bullet.hpp"

Enemy::Enemy(const sf::Texture& InitTexture, 
        float InitSpeed, 
        float InitShootDelay,
        float InitHealth
) : Entity(InitTexture, InitSpeed), shootTime(0.f), 
    shootDelay(InitShootDelay), health(InitHealth),
    dead(false)
{
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Enemy::move(const sf::Time& dt, const sf::Vector2<float>& offset)
{
    sprite.move(dt.asSeconds() * offset);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::Vector2<float> Enemy::getPosition() const
{
    return sprite.getPosition();
}

void Enemy::update(const sf::Time& dt, const sf::Vector2<float>& playerPos, sf::RenderWindow& window)
{
    auto offset = playerPos - this->getPosition();
    float len = std::sqrt(offset.x * offset.x + offset.y * offset.y);
    if (len > 0) offset /= len;

    offset *= speed;
    this->move(dt, offset);
}

void Enemy::getReceivedDamage(float damage) 
{ 
    health -= damage;
    if (health <= 0) 
    {
        health = 0;
        dead = true;
    }
}

bool Enemy::isBulletCollision(const Bullet& bullet)
{ 
    auto bulletHitbox = bullet.getHitbox();
    auto enemyHitbox = sprite.getGlobalBounds();

    return enemyHitbox.intersects(bulletHitbox);
}