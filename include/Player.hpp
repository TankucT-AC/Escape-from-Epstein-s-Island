#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"

class Player : public Entity
{
private:
    float ShootTime;
    float ShootDelay;
public:
    Player(const sf::Texture& InitTexture, float InitSpeed = 300.f);

    virtual void move(const sf::Vector2<float>& offset) override;
    virtual void update(const sf::Time& dt, sf::RenderWindow& window) override;
    virtual void draw(sf::RenderWindow& window) override;

    virtual void moveShootTime(const sf::Time& dt);
    virtual bool isShootTime() const;
    virtual void cooldown();

    virtual sf::Vector2<float> getPosition() const override;
};

#endif