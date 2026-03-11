#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

// Абстракный класс для сущностей
class Entity 
{
protected:
    sf::Sprite sprite;
    float speed;
public:
    Entity(const sf::Texture& InitTexture, 
        float InitSpeed) : sprite(InitTexture), speed(InitSpeed) {}
    virtual ~Entity() = default;

    virtual void move(const sf::Vector2<float>& offset) = 0;
    virtual void update(const sf::Time& dt, sf::RenderWindow& window) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::Vector2<float> getPosition() const = 0;
};

#endif