#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

class Bullet
{
private:
    std::unique_ptr<sf::Sprite> sprite;

    float lifetime;
    float speed;
    sf::Vector2<float> direction;
    bool dead;
public:
    Bullet(const sf::Texture& texture, 
        const sf::Vector2<float>& startPos, 
        const sf::Vector2<float>& dir);

    void update(const sf::Time& dt);
    void draw(sf::RenderWindow& window);

    int getLifetime() const;
    bool isBulletAlive() const;
};

#endif