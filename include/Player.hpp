#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "UpdateContext.hpp"
#include "config.hpp"

class Player : public Entity
{
private:
    float ShootTime;
    float ShootDelay;
public:
    Player(const sf::Texture& InitTexture, 
        float InitSpeed = config::PLAYER_DEFAULT_SPEED, 
        float InitShootDelay = config::PLAYER_DEFAULT_SHOOT_DELAY
    );

    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) override;
    virtual void update(const UpdateContext& ctx) override;
    virtual void draw(sf::RenderWindow& window) override;
    virtual sf::Rect<float> getHitbox() const override;

    virtual void moveShootTime(const sf::Time& dt);
    virtual bool isShootTime() const;
    virtual void cooldown();
};

#endif