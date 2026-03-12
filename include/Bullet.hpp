#ifndef BULLET_HPP
#define BULLET_HPP

#include "Entity.hpp"
#include "config.hpp"

class Bullet : public Entity
{
private:
    float lifetime;
    sf::Vector2<float> direction;
    bool dead;
public:
    Bullet(const sf::Texture& InitTexture, 
        const sf::Vector2<float>& InitStartPos, 
        const sf::Vector2<float>& InitDirection,
        float InitSpeed = config::BULLET_DEFAULT_SPEED,
        float InitLifetime = config::BULLET_DEFAULT_LIFETIME
    );

    virtual void update(const sf::Time& dt, sf::RenderWindow& window) override;
    virtual void draw(sf::RenderWindow& window) override;
    virtual void move(const sf::Vector2<float>& offset) override;
    virtual sf::Vector2<float> getPosition() const override;

    int getLifetime() const;
    bool isBulletAlive() const;
};

#endif