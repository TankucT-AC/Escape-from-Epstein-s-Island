#ifndef BULLET_HPP
#define BULLET_HPP

#include "Entity.hpp"
#include "UpdateContext.hpp"
#include "config.hpp"

class Bullet : public Entity
{
private:
    float damage;
    float lifetime;
    sf::Vector2<float> direction;
    bool dead;
public:
    Bullet(const sf::Texture& InitTexture, 
        const sf::Vector2<float>& InitStartPos, 
        const sf::Vector2<float>& InitDirection,
        float InitDegree = 0.f,
        float InitSpeed = config::BULLET_DEFAULT_SPEED,
        float InitLifetime = config::BULLET_DEFAULT_LIFETIME,
        float InitDamage = config::BULLET_DEFAULT_DAMAGE
    );

    virtual void update(const UpdateContext& ctx) override;
    virtual void draw(sf::RenderWindow& window) override;
    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) override;
    virtual sf::Rect<float> getHitbox() const override;

    int getLifetime() const;
    float getDamage() const;
    bool isBulletAlive() const;

    void setBulletAlive(bool isAlive) { dead = !isAlive; };
};

#endif