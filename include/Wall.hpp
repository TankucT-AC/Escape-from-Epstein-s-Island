#ifndef WALL_HPP
#define WALL_HPP

#include "Entity.hpp"

class Wall : public Entity
{
public:
    Wall(const sf::Texture& InitTexture, sf::Vector2<float> InitPos);

    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) override {};
    virtual void update(const UpdateContext& ctx) override {};
    virtual void draw(sf::RenderWindow& window) override;
};

#endif // WALL_HPP